(() => {
  const API_URL = 'localhost:1337';

  window.getAllSubtopics = () => {
    $.ajax({
      url: `${API_URL}/subtopics`
    }).then((data) => { return data });
  }
})();