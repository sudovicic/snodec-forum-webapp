## SNode.C Forums App

### Description

Simple forums application using [`SNode.C`](https://github.com/VolkerChristian/snode.c) webservers to provide a REST API and consume it in a statically hosted frontend that uses [`page.js`](https://github.com/visionmedia/page.js) for client-side routing. 

### TODOs

- [x] build a real REST API provided by a webserver running SNode.C
- [x] consume that API in the frontend
- [ ] setup webserver to use index.html as an entrypoint for all non-existent requested URLs
    this is needed so that fresh requests (not triggered by `page.js`) to e.g. http://localhost:1337/subtopic/:subtopicId don't fail.
    see: https://github.com/visionmedia/page.js/#server-configuration

### Mandatory Functionality

- [x] Users should be able to register using username and password.
- [x] Users should be able to create subtopics.
- [x] Users should be able to create threads in subtopics.
- [x] Users should be able to create posts in threads.
