# http-server-c
A HTTP server made in C


### Load Scalability

Instead of create a new thread for each new request, I preallocated a thread pool which manages a job queue.
This avoids creating an explosion of threads, keeping resource usage under control.

### File Serving

Enabled the serving of local files through HTTP to effectively service a website.
We now have a tiny limited version of Apache/Nginx 