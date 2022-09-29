(() => {
  // ------------ private api ------------
  var cache = {};

  function get(url, cb) {
    if (cache[url]) return cb(cache[url]);
    $.ajax({
      url: url,
      success: (data) => {
        cache[url] = data;
        cb(data);
      },
      error: (jqXHR, textStatus, errorThrown) => {
        console.error(jqXHR, textStatus, errorThrown);
      },
      dataType: 'text'
    });
  }

  // ------------ public api ------------
  window.init = {
    ctx: (ctx, next) => {
      ctx.data = {};
      ctx.partials = {};
      next();
    }
  };

  window.route = {
    home: (ctx, next) => {
      get('/views/home.html', (html) => {
        ctx.data.index = 0;
        get('/views/partials/subtopics.html', (subtopicsPartial) => {
          ctx.partials.content = html + Hogan.compile(subtopicsPartial).render(ctx.data, ctx.partials);
          next();
        })
      });
    },

    showSubtopic: (ctx, next) => {
      get('/views/subtopic.html', (html) => {
        ctx.data.index = 0;
        get('/views/partials/threads.html', (threadsPartial) => {
          ctx.partials.content = html + Hogan.compile(threadsPartial).render(ctx.data, ctx.partials);
          next();
        })
      });
    },

    showThread: (ctx, next) => {
      get('/views/thread.html', (html) => {
        ctx.data.index = 0;
        get('/views/partials/posts.html', (postsPartial) => {
          ctx.partials.content = html + Hogan.compile(postsPartial).render(ctx.data, ctx.partials);
          next();
        })
      });
    },

    newSubtopic: (ctx, next) => {
      get('/views/newSubtopicForm.html', (html) => {
        ctx.data.index = 1;
        ctx.partials.content = html;
        next();
      });
    },

    newThread: (ctx, next) => {
      get('/views/newThreadForm.html', (html) => {
        ctx.data.index = 0;
        ctx.partials.content = html;
        next();
      });
    },

    newPost: (ctx, next) => {
      get('/views/newPostForm.html', (html) => {
        ctx.data.index = 0;
        ctx.partials.content = html;
        next();
      });
    },

    login: (ctx, next) => {
      get('/views/loginForm.html', (html) => {
        ctx.data.index = 2;
        ctx.partials.content = html;
        next();
      });
    },

    register: (ctx, next) => {
      get('/views/registerForm.html', (html) => {
        ctx.data.index = 3;
        ctx.partials.content = html;
        next();
      });
    },
  };

  window.render = {
    loginStatus: (ctx, next) => {
      get('views/partials/loginStatus.html', (html) => {
        const content = Hogan.compile(html).render(ctx.data, ctx.partials);
        $('#loginStatus').empty().append(content);
        next();
      });
    },
    
    content: (ctx, next) => {
      get('views/partials/content.html', (html) => {
        const content = Hogan.compile(html).render(ctx.data, ctx.partials);
        $('#content').empty().append(content);
        changeActive(ctx.data.index);
        if (typeof done === 'function') done(ctx.data.index);
      });
    },
  };

  window.done = null;
})();
