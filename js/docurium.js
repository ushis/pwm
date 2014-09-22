$(function() {
  var FileListModel = Backbone.Model.extend({
    initialize: function() {
      var docurium = this.get('docurium')
      this.listenTo(docurium, 'change:data', this.extract)
    },

    extract: function() {
      var docurium = this.get('docurium')
      var data = docurium.get('data')
      var version = docurium.get('version')

      // Function groups
      var funs = _.map(data['groups'], function(group, i) {
	var name = group[0]
	var link = groupLink(name, version)
	return {name: name, link: link, num: group[1].length}
      })

      // Types
      var getName = function(type) {
	var name = type[0];
	var link = typeLink(name, version);
	return {link: link, name: name};
      }

      var enums = _.filter(data['types'], function(type) {
	return type[1]['block'] && type[1]['type'] == 'enum';
      }).map(getName)

      var structs = _.filter(data['types'], function(type) {
	return type[1]['block'] && type[1]['type'] != 'enum'
      }).map(getName)

      var opaques = _.filter(data['types'], function(type) {
	return !type[1]['block']
      }).map(getName)

      // File Listing
      var files = _.map(data['files'], function(file) {
	var url = this.github_file(file['file'])
	return {url: url, name: file['file']}
      }, docurium)

      // Examples List
      var examples = []
      if(data['examples'] && (data['examples'].length > 0)) {
	examples = _.map(data['examples'], function(file) {
	  return {name: file[0], path: file[1]}
	})
      }

      this.set('data', {funs: funs, enums: enums, structs: structs, opaques: opaques,
			files: files, examples: examples})
    },
  })

  var FileListView = Backbone.View.extend({
    el: $('#files-list'),

    template:  _.template($('#file-list-template').html()),

    typeTemplate: _.template($('#type-list-template').html()),

    events: {
      'click h3': 'toggleList',
    },

    toggleList: function(e) {
      $(e.currentTarget).next().toggle(100)
      return false
    },

    initialize: function() {
      this.listenTo(this.model, 'change:data', this.render)
    },

    render: function() {
      var data = this.model.get('data')

      var enumList = this.typeTemplate({title: 'Enums', elements: data.enums})
      var structList = this.typeTemplate({title: 'Structs', elements: data.structs})
      var opaquesList = this.typeTemplate({title: 'Opaque Structs', elements: data.opaques})
      var menu = $(this.template({funs: data.funs, files: data.files, examples: data.examples}))

      $('#types-list', menu).append(enumList, structList, opaquesList)

      this.$el.html(menu)
      return this
    },
  })

  var VersionView = Backbone.View.extend({
    el: $('#version'),

    initialize: function() {
      this.listenTo(this.model, 'change:version', this.render)
      this.listenTo(this.model, 'change:name', this.renderName)
      this.title = $('#site-title')
    },

    render: function() {
      var version = this.model.get('version')
      this.$el.text(version)
      this.title.attr('href', '#' + version)
      return this
    },

    renderName: function() {
      var name = this.model.get('name')
      var title = name + ' API'
      this.title.text(title)
      document.title = title
      return this
    },
  })

  var VersionPickerView = Backbone.View.extend({
    el: $('#versions'),

    list: $('#version-list'),

    template: _.template($('#version-picker-template').html()),

    initialize: function() {
      this.listenTo(this.model, 'change:versions', this.render)
    },

    events: {
      'click #version-picker': 'toggleList',
      'click': 'hideList',
    },

    hideList: function() {
      this.list.hide(100)
    },

    toggleList: function(e) {
      $(e.currentTarget).next().toggle(100)
      return false
    },

    render: function() {
      var vers = this.model.get('versions')
      list = this.template({versions: vers})
      this.list.html(list)
      return this
    },
  })

  var ChangelogView = Backbone.View.extend({
    template: _.template($('#changelog-template').html()),

    itemTemplate: _.template($('#changelog-item-template').html()),

    initialize: function() {
      // for every version, show which functions added, removed, changed - from HEAD down
      var versions = this.model.get('versions')
      var sigHist = this.model.get('signatures')

      var lastVer = _.first(versions)

      // fill changelog struct
      var changelog = {}
      for(var i in versions) {
        var version = versions[i]
        changelog[version] = {'deletes': [], 'changes': [], 'adds': []}
      }

      // figure out the adds, deletes and changes
      _.forEach(sigHist, function(func, fname) {
	var lastv = _.last(func.exists)
	var firstv = _.first(func.exists)
	changelog[firstv]['adds'].push(fname)

	// figure out where it was deleted or changed
	if (lastv && (lastv != lastVer)) {
	  var vi = _.indexOf(versions,lastv)
	  var delv = versions[vi-1]
	  changelog[delv]['deletes'].push(fname)

	  _.forEach(func.changes, function(_, v) {
	    changelog[v]['changes'].push(fname)
	  })
	}
      })

      var vers = _.map(versions, function(version) {
	var deletes = changelog[version]['deletes']
	deletes.sort()

	var additions = changelog[version]['adds']
	additions.sort()
	var adds = _.map(additions, function(add) {
          var gname = this.model.groupOf(add)
	  return {link: functionLink(gname, add, version), text: add}
	}, this)

	return {title: version, listing: this.itemTemplate({dels: deletes, adds: adds})}
      }, this)

      this.el = this.template({versions: vers})
    },

    render: function() {
      return this
    }
  })

  var FunctionModel = Backbone.Model.extend({
    initialize: function() {
      var gname = this.get('gname')
      var fname = this.get('fname')
      var docurium = this.get('docurium')

      var group = docurium.getGroup(gname)

      var fdata = docurium.get('data')['functions']
      var functions = group[1]

      // Function Arguments
      var args = _.map(fdata[fname]['args'], function(arg) {
	return {link: this.hotLink(arg.type), name: arg.name, comment: arg.comment}
      }, docurium)

      var data = fdata[fname]
      // function return value
      var ret = data['return']
      var returns = {link: docurium.hotLink(ret.type), comment: ret.comment}
      // function signature
      var sig = docurium.hotLink(ret.type) + ' ' + fname + '(' + data['argline'] + ');'
      // version history
      var sigHist = docurium.get('signatures')[fname]
      var version = docurium.get('version')
      var sigs = _.map(sigHist.exists, function(ver) {
	var klass = []
	if (sigHist.changes[ver])
	  klass.push('changed')
	if (ver == version)
	  klass.push('current')

	return {url: '#' + functionLink(gname, fname, ver), name: ver, klass: klass.join(' ')}
      })
      // GitHub link
      var fileLink = docurium.github_file(data.file, data.line, data.lineto)
      // link to the group
      var version = docurium.get('version')
      var alsoGroup = '#' + groupLink(group[0], version)
      var alsoLinks = _.map(functions, function(f) {
	return {url: '#' + functionLink(gname, f, version), name: f}
      })

      this.set('data', {name: fname, data: data, args: args, returns: returns, sig: sig,
			sigs: sigs, fileLink: fileLink, groupName: gname,
			alsoGroup: alsoGroup, alsoLinks: alsoLinks})
    }
  })

  var FunctionView = Backbone.View.extend({
    template: _.template($('#function-template').html()),
    argsTemplate: _.template($('#function-args-template').html()),

    render: function() {
      document.body.scrollTop = document.documentElement.scrollTop = 0;
      var data = this.model.get('data')
      data.argsTemplate = this.argsTemplate
      var cont = this.template(data)

      this.el = cont
      return this
    },
  })

  var GroupCollection = Backbone.Collection.extend({
    initialize: function(o) {
      this.docurium = o.docurium
      this.listenTo(this.docurium, 'change:data', this.refill)
    },

    refill: function(o, doc) {
      var data = o.changed.data
      var sigHist = this.docurium.get('signatures')
      var version = this.docurium.get('version')

      var groups = _.map(data.groups, function(group) {
	var gname = group[0]
	var funs = _.map(group[1], function(fun) {
	  var klass = ''
	  if (sigHist[fun].changes[version])
	    klass = 'changed'

	  if (version == _.first(sigHist[fun].exists))
	    klass = 'introd'

	  return {name: fun, url: '#' + functionLink(gname, fun, version), klass: klass}
	})
	return {name: gname, funs: funs}
      })

      this.reset(groups)
    },
  })

  var MainListView = Backbone.View.extend({
    template: _.template($('#index-template').html()),

    initialize: function() {
      this.listenTo(this.collection, 'reset', this.render)
    },

    render: function() {
      this.el = this.template({groups: this.collection.toJSON()})
      this.trigger('redraw')
      return this
    },
  })

  var TypeModel = Backbone.Model.extend({
    initialize: function() {
      var typename = this.get('typename')
      var docurium = this.get('docurium')
      var version = docurium.get('version')
      var types = docurium.get('data')['types']
      var tdata = _.find(types, function(g) {
	return g[0] == typename
      })
      var tname = tdata[0]
      var data = tdata[1]

      var toPair = function(fun) {
	var gname = this.groupOf(fun)
	var url = '#' + functionLink(gname, fun, version)
	return {name: fun, url: url}
      }

      var returns = _.map(data.used.returns, toPair, docurium)
      var needs = _.map(data.used.needs, toPair, docurium)
      var fileLink = {name: data.file, url: docurium.github_file(data.file, data.line, data.lineto)}

      // so it doesn't look crap, we build up a block with fields
      // without a comment
      var had_comment = false
      var blocks = []
      var tmp = []
      _.each(data.fields, function(f) {
	if (had_comment) {
	  blocks.push(tmp)
	  tmp = []
	}

	tmp.push(f)
	had_comment = f.comments
      })
      blocks.push(tmp)

      this.set('data', {tname: tname, data: data, blocks: blocks, returns: returns, needs: needs, fileLink: fileLink})
    }
  })

  var TypeView = Backbone.View.extend({
    enumTemplate: _.template($('#enum-template').html()),
    structTemplate: _.template($('#struct-template').html()),
    usesTemplate: _.template($('#uses-template').html()),

    render: function() {
      var type = this.model.get('data')
      var uses = this.usesTemplate(type)

      var template = type.data.type == 'struct' ? this.structTemplate : this.enumTemplate
      var content = template({type: type, uses: uses})

      this.el = content
      return this
    }
  })

  var GroupView = Backbone.View.extend({
    template: _.template($('#group-template').html()),

    initialize: function(o) {
      var group = o.group
      var gname = group[0]
      var fdata = o.functions
      var version = o.version

      this.functions = _.map(group[1], function(name) {
	var url = '#' + functionLink(gname, name, version)
	var d = fdata[name]
	return {name: name, url: url, returns: d['return']['type'], argline: d['argline'],
		description: d['description'], comments: d['comments'], args: d['args']}
      })
    },

    render: function() {
      var content = this.template({gname: this.gname, functions: this.functions})

      this.el = content
      return this
    },
  })

  var SearchFieldView = Backbone.View.extend({
    tagName: 'input',

    el: $('#search-field'),

    events: {
      'keyup': function() {
	this.trigger('keyup')
	if (this.$el.val() == '')
	  this.trigger('empty')
      }
    },
  })

  var SearchCollection = Backbone.Collection.extend({
    defaults: {
      value: '',
    },

    initialize: function(o) {
      this.field = o.field
      this.docurium = o.docurium

      this.listenTo(this.field, 'keyup', this.keyup)
    },

    keyup: function() {
      var newValue = this.field.$el.val()
      if (this.value == newValue || newValue.length < 3)
	return

      this.value = newValue
      this.refreshSearch()
    },

    refreshSearch: function() {
      var docurium = this.docurium
      var value = this.value

      var data = docurium.get('data')
      var searchResults = []

      var version = docurium.get('version')
      // look for functions (name, comment, argline)
      _.forEach(data.functions, function(f, name) {
	var gname = docurium.groupOf(name)
	// look in the function name first
        if (name.search(value) > -1) {
	  var gl = functionLink(gname, name, version)
	  var url = '#' + gl
	  searchResults.push({url: url, name: name, match: 'function', navigate: gl})
	  return
        }

	// if we didn't find it there, let's look in the argline
        if (f.argline && f.argline.search(value) > -1) {
	  var gl = functionLink(gname, name, version)
	  var url = '#' + gl
          searchResults.push({url: url, name: name, match: f.argline, navigate: gl})
        }
      })

      // look for types
      data.types.forEach(function(type) {
        var name = type[0]
	var tl = typeLink(name, version)
	var url = '#' + tl
        if (name.search(value) > -1) {
          searchResults.push({url: url, name: name, match: type[1].type, navigate: tl})
        }
      })

      this.reset(searchResults)
    },
  })

  var SearchView = Backbone.View.extend({
    template: _.template($('#search-template').html()),

    // initialize: function() {
    //   this.listenTo(this.model, 'reset', this.render)
    // },

    render: function() {
      var content = this.template({results: this.collection.toJSON()})
      this.el = content
     }
  })

  var MainView = Backbone.View.extend({
    el: $('#content'),

    setActive: function(view) {
      view.render()

      if (this.activeView) {
	this.stopListening()
	this.activeView.remove()
      }

      this.activeView = view
      // make sure we know when the view wants to render again
      this.listenTo(view, 'redraw', this.render)

      this.$el.html(view.el)

      // move back to the top when we switch views
      document.body.scrollTop = document.documentElement.scrollTop = 0;
    },

    render: function() {
      this.$el.html(this.activeView.el)
    },
  })

  // our document model - stores the datastructure generated from docurium
  var Docurium = Backbone.Model.extend({

    defaults: {'version': 'unknown'},

    initialize: function() {
      this.loadVersions()
      this.bind('change:version', this.loadDoc)
    },

    loadVersions: function() {
      $.getJSON("project.json").then(function(data) {
        docurium.set({'versions': data.versions, 'github': data.github, 'signatures': data.signatures, 'name': data.name})
        docurium.setVersion()
      })
    },

    setVersion: function (version) {
      if(!version) {
        version = _.first(docurium.get('versions'))
      }
      docurium.set({version: version})
    },

    loadDoc: function() {
      version = this.get('version')
      $.getJSON(version + '.json').then(function(data) {
        docurium.set({data: data})
      })
    },

    getGroup: function(gname) {
      var groups = docurium.get('data')['groups']
      return _.find(groups, function(g) {
	return g[0] == gname
      })
    },

    // look for structs and link them 
    hotLink: function(text) {
      types = this.get('data')['types']
      var version = this.get('version')
      for(var i=0; i<types.length; i++) {
        type = types[i]
        typeName = type[0]
        typeData = type[1]
        re = new RegExp(typeName + ' ', 'gi');
        var link = $('<a>').attr('href', '#' + typeLink(typeName, version)).append(typeName)[0]
        text = text.replace(re, link.outerHTML + ' ')
      }
      return text
    },

    groupOf: function (func) {
      return this.get('data')['functions'][func]['group']
    },

    github_file: function(file, line, lineto) {
      var data = this.get('data')
      url = ['https://github.com', docurium.get('github'),
	     'blob', docurium.get('version'), data.prefix, file].join('/')
      if(line) {
        url += '#L' + line.toString()
        if(lineto) {
          url += '-' + lineto.toString()
        }
      } else {
        url += '#files'
      }

      return url
    },
  })

  var Workspace = Backbone.Router.extend({

    routes: {
      "":                             "index",
      ":version":                     "main",
      ":version/group/:group":        "group",
      ":version/type/:type":          "showtype",
      ":version/group/:group/:func":  "groupFun",
      ":version/search/:query":       "search",
      "p/changelog":                  "changelog",
    },

    initialize: function(o) {
      this.doc = o.docurium
      this.search = o.search
      this.mainView = o.mainView
      this.groups = o.groups
    },

    index: function() {
      // set the default version
      this.doc.setVersion()
      // and replate our URL with it, to avoid a back-button loop
      this.navigate(this.doc.get('version'), {replace: true, trigger: true})
    },

    main: function(version) {
      this.doc.setVersion(version)
      var view = new MainListView({collection: this.groups})
      this.mainView.setActive(view)
    },

    group: function(version, gname) {
      this.doc.setVersion(version)
      var group = this.doc.getGroup(gname)
      var fdata = this.doc.get('data')['functions']
      var version = this.doc.get('version')
      var view = new GroupView({group: group, functions: fdata, version: version})
      this.mainView.setActive(view)
    },

    groupFun: function(version, gname, fname) {
      this.doc.setVersion(version)
      var model = new FunctionModel({docurium: this.doc, gname: gname, fname: fname})
      var view = new FunctionView({model: model})
      this.mainView.setActive(view)
    },

    showtype: function(version, tname) {
      this.doc.setVersion(version)
      var model = new TypeModel({docurium: this.doc, typename: tname})
      var view = new TypeView({model: model})
      this.mainView.setActive(view)
    },

    search: function(version, query) {
      this.doc.setVersion(version)
      var view = new SearchView({collection: this.search})
      $('#search-field').val(query).keyup()
      this.mainView.setActive(view)
    },

    changelog: function(version, tname) {
      // let's wait to process it until it's asked, and let's only do
      // it once
      if (this.changelogView == undefined) {
	this.changelogView = new ChangelogView({model: this.doc})
      }
      this.doc.setVersion()
      this.mainView.setActive(this.changelogView)
    },
  });

  function functionLink(gname, fname, version) {
      return version + "/group/" + gname + '/' + fname
  }

  function groupLink(gname, version) {
      return version + "/group/" + gname
  }

  function typeLink(tname, version) {
    return version + "/type/" + tname
  }

  function searchLink(term, version) {
    return version + "/search/" + term
  }

  //_.templateSettings.variable = 'rc'

  var docurium = new Docurium

  var searchField = new SearchFieldView({id: 'search-field'})
  var searchCol = new SearchCollection({docurium: docurium, field: searchField})
  var groupCol = new GroupCollection({docurium: docurium})

  var mainView = new MainView()

  var router = new Workspace({docurium: docurium, search: searchCol, mainView: mainView,
			      groups: groupCol})

  searchField.on('empty', function() {
    router.navigate(docurium.get('version'), {trigger: true})
  })

  docurium.once('change:data', function() {Backbone.history.start()})

  var fileList = new FileListModel({docurium: docurium})
  var fileListView = new FileListView({model: fileList})
  var versionView = new VersionView({model: docurium})
  var versionPickerView = new VersionPickerView({model: docurium})

  searchCol.on('reset', function(col, prev) {
    if (col.length == 1) {
      router.navigate(col.pluck('navigate')[0], {trigger: true, replace: true})
    } else {
      var version = docurium.get('version')
      // FIXME: this keeps recreating the view
      router.navigate(searchLink(col.value, version), {trigger: true})
    }
  })
})
