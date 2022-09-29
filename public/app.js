// @see https://github.com/visionmedia/page.js/issues/537#issuecomment-492755878
page.configure({window: window});

page('/*', init.ctx, middleware.checkAuth);

page('/home', middleware.loadSubtopics, route.home);
page('/subtopic/:subtopicId', middleware.loadThreads, route.showSubtopic);
page('/subtopics/new', route.newSubtopic);
page('/subtopic/:subtopicId/thread/:threadId', middleware.loadThreads, middleware.loadPosts, route.showThread);
page('/subtopic/:id/threads/new', route.newThread);
// TODO implement
page('/subtopic/:subtopicId/threads/:threadId/posts/new', route.newPost);

page('/login', route.login);
page('/logout', middleware.logout, () => page.redirect('/home'));
page('/register', route.register);

page('/*', render.loginStatus, render.content);

page.start();