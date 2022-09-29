window.middleware = {
    // TODO: redirect to /login for certain routes if no session exists?
    checkAuth: async (ctx, next) => {
        ctx.data.session = $.cookie("sessionCookie") ? true : false;
        next();
    },

    logout: async (ctx, next) => {
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
            const result = await window.apiService.getAllThreadsOfSubtopic(ctx.params.subtopicId);
            const threads = JSON.parse(result);
            ctx.state.threads = threads;
            ctx.data.threads = threads;
            ctx.state.subtopicId = ctx.params.subtopicId;
            ctx.data.subtopicId = ctx.params.subtopicId;

            if (ctx.params.threadId) {
                const thread = threads.find((t) => t.thread_id === ctx.params.threadId);
                if (thread) {
                    ctx.state.thread = thread;
                    ctx.data.thread = thread;
                } 
            }

            ctx.save();
        } else {
            ctx.data.threads = ctx.state.threads;
            ctx.data.subtopicId = ctx.state.subtopicId;
        }
        next();
    },

    loadPosts: async (ctx, next) => {
        if (!ctx.state.posts) {
            const { posts } = await window.apiService.getAllPostsOfThread(ctx.params.subtopicId, ctx.params.threadId);
            ctx.state.posts = posts;
            ctx.data.posts = posts;
            ctx.state.subtopicId = ctx.params.subtopicId;
            ctx.data.subtopicId = ctx.params.subtopicId;
            ctx.state.threadId = ctx.params.threadId;
            ctx.data.threadId = ctx.params.threadId;
            ctx.save();
        } else {
            ctx.data.posts = ctx.state.posts;
            ctx.data.subtopicId = ctx.state.subtopicId;
            ctx.data.threadId = ctx.state.threadId;
        }
        next();
    },
}