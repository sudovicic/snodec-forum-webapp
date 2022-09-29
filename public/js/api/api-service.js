(() => {
  const API_URL = 'http://127.0.0.1:8080';

  window.apiService = {
    logout: async () => {
      return await $.ajax({
        url: `${API_URL}/logout`,
        xhrFields: {
          withCredentials: true
        }
      });
    },

    getAllSubtopics: async () => {
      return await $.ajax({url: `${API_URL}/subtopics`});
    },
  
    getAllThreadsOfSubtopic: async (subtopicId) => {
      return await $.ajax({url: `${API_URL}/subtopic/${subtopicId}/threads`})
    },
  
    getAllPostsOfThread: async (subtopicId, threadId) => {
      return await $.ajax({url: `${API_URL}/subtopic/${subtopicId}/threads/${threadId}/posts`});
    },
  };
})();
