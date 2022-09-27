$(() => {
    var navLinks = $('#navigation a');
  
    navLinks.on('click', (e) => {
      changeActive(navLinks.index(e.target));
    });
  
    window.changeActive = (index) => {
      navLinks.removeClass('current').eq(index).addClass('current');
    }
  });
