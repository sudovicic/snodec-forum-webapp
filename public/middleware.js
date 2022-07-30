window.middleware = {
    loadSubtopics: async (ctx, next) => {
        if (!ctx.state.subtopics) {
            // cache miss
            const { subtopics } = await window.apiService.getAllSubtopics(); // TODO: if needed, add JSON parsing once the real API is used
            ctx.state.subtopics = subtopics;
            ctx.data.subtopics = subtopics;
            ctx.save();
        } else {
            // user used browser back/forward buttons for navigation --> cache hit
            ctx.data.subtopics = ctx.state.subtopics;
        }
        next();
    },
}