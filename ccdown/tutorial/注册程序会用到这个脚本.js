self.addEventListener('install', (event) => {
  event.waitUntil(
    caches.open('my-cache').then((cache) => {
      return cache.addAll([
        // 这里放资源缓存
        //@shc 有些网站对iframe所请求的资源进行了限制
        //请找到网站以后测试一下，是否会有图片之类的没加载出来
        //有的话，把它的缓存放到这个JS列表里
        //不会可以问gpt
      ]);
    })
  );
});

self.addEventListener('fetch', (event) => {
  const url = new URL(event.request.url);

  if (url.hostname === 'github.com') {
    event.respondWith(
      fetch(event.request).then((response) => {
        // 克隆响应并修改头部
        const modifiedResponse = response.clone();
        
        const modifiedHeaders = new Headers(modifiedResponse.headers);
        // 删除或修改 X-Frame-Options 和 Content-Security-Policy 头部
        //@shc 这两行最关键，通过篡改请求头破解限制
        //你需要确认一下，主要的几个资源网站会利用哪些响应头禁用iframe
        //如果找到其他的，请学一下禁用它的方法，
        //放在31-33行之间
        modifiedHeaders.delete('X-Frame-Options');
        modifiedHeaders.delete('Content-Security-Policy');
        
        return new Response(modifiedResponse.body, {
          status: modifiedResponse.status,
          statusText: modifiedResponse.statusText,
          headers: modifiedHeaders
        });
      })
    );
  } else {
    event.respondWith(fetch(event.request));  // 对其他请求保持默认行为
  }
});
