(() => {
  const API_URL = 'http://localhost:8080';

  window.apiService = {
    logout: async () => {
      return await $.ajax({
        method: 'POST',
        url: `${API_URL}/logout`
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
