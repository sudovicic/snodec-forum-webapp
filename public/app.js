// @see https://github.com/visionmedia/page.js/issues/537#issuecomment-492755878
page.configure({window: window});

page('/*', init.ctx, middleware.checkAuth);

page('/home', middleware.loadSubtopics, route.home);
page('/subtopic/:id', middleware.loadThreads, route.showSubtopic);
page('/thread/:id', middleware.loadPosts, route.showThread);
page('/subtopic/:id/threads/new', route.newThread);
page('/subtopics/new', route.newSubtopic);
page('/login', route.login);
page('/logout', middleware.logout, () => page.redirect('/home'));
page('/register', route.register);

page('/*', middleware.checkAuth, render.loginStatus, render.content);

page.start();