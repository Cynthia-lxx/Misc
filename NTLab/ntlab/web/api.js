/* Minimal REST helper with unified error unwrapping. */

export async function apiGet(path, params = {}) {
  const url = new URL(path, location.origin);
  Object.entries(params).forEach(([k, v]) => {
    if (v !== undefined && v !== null) url.searchParams.set(k, v);
  });
  const res = await fetch(url, { headers: { accept: "application/json" } });
  return unwrap(res);
}

export async function apiPost(path, body) {
  const res = await fetch(path, {
    method: "POST",
    headers: { "content-type": "application/json" },
    body: JSON.stringify(body),
  });
  return unwrap(res);
}

export async function apiUpload(path, file) {
  const form = new FormData();
  form.append("file", file, file.name);
  const res = await fetch(path, { method: "POST", body: form });
  return unwrap(res);
}

async function unwrap(res) {
  const data = await res.json().catch(() => ({}));
  if (!res.ok) {
    const err = data?.error ?? {};
    const e = new Error(err.message || `HTTP ${res.status}`);
    e.code = err.code || "http_error";
    e.detail = err.detail || {};
    throw e;
  }
  return data;
}
