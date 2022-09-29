// @see https://github.com/visionmedia/page.js/issues/537#issuecomment-492755878
page.configure({window: window});

page('/*', init.ctx, middleware.checkAuth);

page('/home', middleware.loadSubtopics, route.home);
page('/subtopic/:subtopicId', middleware.loadThreads, route.showSubtopic);
page('/subtopics/new', route.newSubtopic);
page('/subtopic/:subtopicId/thread/:threadId', middleware.loadThreads, middleware.loadPosts, route.showThread);
page('/subtopic/:subtopicId/threads/new', route.newThread);
page('/subtopic/:subtopicId/threads/:threadId/posts/new', middleware.loadThreads, middleware.loadPosts, route.newPost);

page('/login', route.login);
page('/logout', middleware.logout, () => page.redirect('/home'));
page('/register', route.register);

page('/*', render.header, init.navLinks, render.content);

page.start();