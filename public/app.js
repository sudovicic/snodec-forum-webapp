// @see https://github.com/visionmedia/page.js/issues/537#issuecomment-492755878
page.configure({window: window});

page('/*', init.ctx);

page('/home', middleware.loadSubtopics, route.home);
page('/subtopics/new', route.newSubtopic);
page('/login', route.login);
page('/register', route.register);

page('/*', render.content);

page.start();