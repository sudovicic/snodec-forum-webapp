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
      ctx.data = {
        activeNavLinkId: '#home',
      };
      ctx.partials = {};
      next();
    },

    navLinks: (ctx, next) => {
      const navLinks = $('#navigation a');
      navLinks.on('click', (e) => {
        const id = "#" + e.target.id;
        changeActive(id);
      });
      window.changeActive = (id) => {
        navLinks.removeClass('current');
        navLinks.filter(id).addClass('current');
      };
      next();
    },
  };

  window.route = {
    home: (ctx, next) => {
      get('/views/home.html', (html) => {
        ctx.data.activeNavLinkId = '#home';
        get('/views/partials/subtopics.html', (subtopicsPartial) => {
          ctx.partials.content = html + Hogan.compile(subtopicsPartial).render(ctx.data, ctx.partials);
          next();
        })
      });
    },

    showSubtopic: (ctx, next) => {
      get('/views/subtopic.html', (html) => {
        ctx.data.activeNavLinkId = '#home';
        get('/views/partials/threads.html', (threadsPartial) => {
          ctx.partials.content = html + Hogan.compile(threadsPartial).render(ctx.data, ctx.partials);
          next();
        })
      });
    },

    showThread: (ctx, next) => {
      get('/views/thread.html', (html) => {
        ctx.data.activeNavLinkId = '#home';
        get('/views/partials/posts.html', (postsPartial) => {
          ctx.partials.content = html + Hogan.compile(postsPartial).render(ctx.data, ctx.partials);
          next();
        })
      });
    },

    newSubtopic: (ctx, next) => {
      get('/views/newSubtopicForm.html', (html) => {
        ctx.data.activeNavLinkId = '#newSubtopic';
        ctx.partials.content = html;
        next();
      });
    },

    newThread: (ctx, next) => {
      get('/views/newThreadForm.html', (html) => {
        ctx.data.activeNavLinkId = '#home';
        ctx.data.subtopicId = ctx.params.subtopicId;
        ctx.partials.content = html;
        next();
      });
    },

    newPost: (ctx, next) => {
      get('/views/newPostForm.html', (html) => {
        ctx.data.activeNavLinkId = '#home';
        ctx.partials.content = html;
        next();
      });
    },

    login: (ctx, next) => {
      get('/views/loginForm.html', (html) => {
        ctx.data.activeNavLinkId = '#login';
        ctx.partials.content = html;
        next();
      });
    },

    register: (ctx, next) => {
      get('/views/registerForm.html', (html) => {
        ctx.data.activeNavLinkId = '#register';
        ctx.partials.content = html;
        next();
      });
    },
  };

  window.render = {
    header: (ctx, next) => {
      get('views/partials/header.html', (defaultHeaderHtml) => {
        const defaultHeaderContent = Hogan.compile(defaultHeaderHtml).render(ctx.data, ctx.partials);
        $('header').empty().append(defaultHeaderContent);

        get('views/partials/navigation.html', (navigationHtml) => {
          const navigationContent = Hogan.compile(navigationHtml).render(ctx.data, ctx.partials);
          $('header').prepend(navigationContent);

          get('views/partials/loginStatus.html', (loginStatusHtml) => {
            const loginStatusContent = Hogan.compile(loginStatusHtml).render(ctx.data, ctx.partials);
            $('header').prepend(loginStatusContent);
          });

          next();
        });
      });
    },

    content: (ctx, next) => {
      get('views/partials/content.html', (html) => {
        const content = Hogan.compile(html).render(ctx.data, ctx.partials);
        $('#content').empty().append(content);
        changeActive(ctx.data.activeNavLinkId);
        if (typeof done === 'function') {
          console.log(done);
          done(ctx.data.activeNavLinkId);
        }
      });
    },
  };

  window.done = null;
})();
