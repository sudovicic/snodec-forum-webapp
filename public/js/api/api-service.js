(() => {
  const API_URL = 'http://127.0.0.1:8080';

  window.apiService = {
    getAllSubtopics: async () => {
      return await $.ajax({url: `${API_URL}/subtopics`});
    },
  
    // getSubtopic: (subtopicId) => {
    //   return await $.ajax({url: `${API_URL}/subtopic/${subtopicId}`});
    // },
  
    getAllThreadsOfSubtopic: async (subtopicId) => {
      return await $.ajax({url: `${API_URL}/subtopic/${subtopicId}/threads`})
    },
  
    getAllPostsOfThread: async (threadId) => {
      // $.ajax({
      //   url: `${API_URL}/threads/${threadId}/posts`
      // }).then((data) => { return data });
      return new Promise(
        (resolve) => setTimeout(
          () => resolve({ 'posts': [{'id': 0, 'content': 'my post content'}, {'id': 1, content: 'another post content'}]}),
          500,
        )
      );
    },
  
    // TODO: figure out if we need to redirect on the client in some way
    // createSubtopic: () => {
    //   return $.ajax({
    //     url: `${API_URL}/subtopics/new`,
    //     method: 'POST'
    //   });
    // },
  
    createThreadInSubtopic: async (subtopicId) => {
      return $.ajax({
        url: `${API_URL}/subtopic/${subtopicId}/threads/new`,
        method: 'POST'
      });
    },
  
    createPostInThread: async (subtopicId, threadId) => {
      return $.ajax({
        url: `${API_URL}/subtopic/${subtopicId}/threads/${threadId}/posts/new`,
        method: 'POST'
      });
    },
  };
})();
