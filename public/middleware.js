window.middleware = {
    // TODO: redirect to /login for certain routes if no session exists?
    checkAuth: (ctx, next) => {
        ctx.data.session = $.cookie("sessionCookie") ? true : false;
        next();
    },

    logout: (ctx, next) => {
        ctx.data.session = null;
        $.removeCookie("sessionCookie");
        next();
    }, 

    loadSubtopics: async (ctx, next) => {
        if (!ctx.state.subtopics) {
            // cache miss
            const result = await window.apiService.getAllSubtopics();
            const subtopics = JSON.parse(result);
            ctx.state.subtopics = subtopics;
            ctx.data.subtopics = subtopics;
            ctx.save();
        } else {
            // user used browser back/forward buttons for navigation --> cache hit
            ctx.data.subtopics = ctx.state.subtopics;
        }
        next();
    },

    loadThreads: async (ctx, next) => {
        if (!ctx.state.threads) {
            const result = await window.apiService.getAllThreadsOfSubtopic(ctx.params.id);
            const threads = JSON.parse(result);
            ctx.state.threads = threads;
            ctx.data.threads = threads;
            ctx.state.subtopicId = ctx.params.id;
            ctx.data.subtopicId = ctx.params.id;
            ctx.save();
        } else {
            ctx.data.threads = ctx.state.threads;
            ctx.data.subtopicId = ctx.state.subtopicId;
        }
        next();
    },

    loadPosts: async (ctx, next) => {
        if (!ctx.state.posts) {
            const { posts } = await window.apiService.getAllPostsOfThread(ctx.params.id);
            ctx.state.posts = posts;
            ctx.data.posts = posts;
            ctx.save();
        } else {
            ctx.data.posts = ctx.state.posts;
        }
        next();
    },
}