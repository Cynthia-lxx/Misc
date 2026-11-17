/* 带进度的加载工具（osu ExpandingBar 风格）。
 *
 * - apiGetProgress / fetchBytesProgress / fetchBlobProgress：fetch + ReadableStream
 *   按 Content-Length 分段回报进度（服务器未给长度时以 indeterminate 模式展示）。
 * - apiUploadProgress：XMLHttpRequest upload.onprogress（fetch 无上传进度）。
 * - createLoadbar：3px 细进度条 + 百分比标签，finish() 后 300ms 淡出移除。
 */

const frame = (fn) => requestAnimationFrame(fn);

/** 细进度条：{ el, set(loaded, total), indeterminate(), finish() } */
export function createLoadbar(label = "加载中…") {
  const el = document.createElement("div");
  el.className = "loadbar";
  el.setAttribute("role", "progressbar");
  const fill = document.createElement("i");
  const text = document.createElement("span");
  text.textContent = label;
  el.append(fill, text);

  let finished = false;
  const set = (loaded, total) => {
    if (finished) return;
    if (total > 0) {
      const p = Math.min(1, loaded / total);
      fill.style.width = `${Math.max(2, p * 100)}%`;
      fill.classList.remove("indet");
      el.setAttribute("aria-valuenow", String(Math.round(p * 100)));
      text.textContent = `${label} ${Math.round(p * 100)}%`;
    } else {
      fill.style.width = "30%";
      fill.classList.add("indet");
    }
  };

  const indeterminate = () => {
    fill.style.width = "30%";
    fill.classList.add("indet");
  };

  const finish = () => {
    if (finished) return;
    finished = true;
    fill.style.width = "100%";
    fill.classList.remove("indet");
    el.setAttribute("aria-valuenow", "100");
    text.textContent = `${label} 100%`;
    frame(() => frame(() => {
      el.classList.add("done");
      window.setTimeout(() => el.remove(), 450);
    }));
  };

  return { el, set, indeterminate, finish };
}

/** 分段读取响应体（进度回调 loaded/total；total 未知为 0）。 */
async function readBody(res, onProgress) {
  const total = Number(res.headers.get("content-length")) || 0;
  if (!res.body) {
    const buf = await res.arrayBuffer();
    onProgress?.(buf.byteLength, buf.byteLength);
    return buf;
  }
  const reader = res.body.getReader();
  const chunks = [];
  let loaded = 0;
  for (;;) {
    const { done, value } = await reader.read();
    if (done) break;
    chunks.push(value);
    loaded += value.length;
    onProgress?.(loaded, total);
  }
  const buf = await new Blob(chunks).arrayBuffer();
  onProgress?.(buf.byteLength, buf.byteLength || total);
  return buf;
}

function throwHttpError(res, data) {
  const err = data?.error ?? {};
  const e = new Error(err.message || `HTTP ${res.status}`);
  e.code = err.code || "http_error";
  e.detail = err.detail ?? {};
  throw e;
}

/** JSON GET，带读取进度。 */
export async function apiGetProgress(path, params = {}, onProgress) {
  const url = new URL(path, location.origin);
  Object.entries(params).forEach(([k, v]) => {
    if (v !== undefined && v !== null) url.searchParams.set(k, v);
  });
  const res = await fetch(url, { headers: { accept: "application/json" } });
  if (!res.ok) {
    const data = await res.json().catch(() => ({}));
    throwHttpError(res, data);
  }
  const buf = await readBody(res, onProgress);
  return JSON.parse(new TextDecoder().decode(buf));
}

/** 二进制 GET，带读取进度。 */
export async function fetchBytesProgress(path, onProgress) {
  const res = await fetch(path);
  if (!res.ok) {
    const data = await res.json().catch(() => ({}));
    throwHttpError(res, data);
  }
  return new Uint8Array(await readBody(res, onProgress));
}

/** Blob GET（打包下载等），带读取进度。 */
export async function fetchBlobProgress(path, onProgress) {
  const res = await fetch(path);
  if (!res.ok) {
    const data = await res.json().catch(() => ({}));
    throwHttpError(res, data);
  }
  return new Blob([await readBody(res, onProgress)]);
}

/** POST 上传（XMLHttpRequest，带上传进度）。 */
export function apiUploadProgress(path, file, onProgress) {
  return new Promise((resolve, reject) => {
    const xhr = new XMLHttpRequest();
    xhr.open("POST", path);
    xhr.responseType = "json";
    xhr.upload.onprogress = (e) => onProgress?.(e.loaded, e.total);
    xhr.onload = () => {
      const data = xhr.response ?? {};
      if (xhr.status >= 200 && xhr.status < 300) resolve(data);
      else reject(Object.assign(new Error(data?.error?.message || `HTTP ${xhr.status}`), {
        code: data?.error?.code || "http_error",
      }));
    };
    xhr.onerror = () => reject(Object.assign(new Error("网络错误"), { code: "network_error" }));
    const form = new FormData();
    form.append("file", file, file.name);
    xhr.send(form);
  });
}
