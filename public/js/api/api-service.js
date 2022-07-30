(() => {
  const API_URL = 'localhost:1337';

  window.apiService = {
    getAllSubtopics: () => {
      // $.ajax({
      //   url: `${API_URL}/subtopics`
      // }).then((data) => { return data });
      return new Promise(
        (resolve) => setTimeout(
          () => resolve({ 'subtopics': [{'id': 0, 'name': 'subtopic 1'}, {'id': 1, name: 'subtopic 2'}, {'id': 2, name: 'subtopic 3'}]}),
          500,
        )
      );
    },
  
    getSubtopic: (subtopicId) => {
      $.ajax({
        url: `${API_URL}/subtopic/${subtopicId}`
      }).then((data) => { return data });
    },
  
    getAllThreadsOfSubtopic: (subtopicId) => {
      $.ajax({
        url: `${API_URL}/subtopic/${subtopicId}/threads`
      }).then((data) => { return data });
    },
  
    getAllPostsOfThread: (subtopicId, threadId) => {
      $.ajax({
        url: `${API_URL}/subtopic/${subtopicId}/threads/${threadId}/posts`
      }).then((data) => { return data });
    },
  
    // TODO: figure out if we need to redirect on the client in some way
    createSubtopic: () => {
      return $.ajax({
        url: `${API_URL}/subtopics/new`,
        method: 'POST'
      });
    },
  
    createThreadInSubtopic: (subtopicId) => {
      return $.ajax({
        url: `${API_URL}/subtopic/${subtopicId}/threads/new`,
        method: 'POST'
      });
    },
  
    createPostInThread: (subtopicId, threadId) => {
      return $.ajax({
        url: `${API_URL}/subtopic/${subtopicId}/threads/${threadId}/posts/new`,
        method: 'POST'
      });
    },
  
    register: (username, password) => {
      return $.ajax({
        url: `${API_URL}/register`,
        method: 'POST'
      });
    },
  
    login: (username, password) => {
      return $.ajax({
        url: `${API_URL}/login`,
        method: 'POST'
      });
    }
  };
})();