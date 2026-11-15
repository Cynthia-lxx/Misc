/* NTLab SPA bootstrap: import vendor elements, register pages, start router. */

// vendor custom elements (ES modules self-register on import)
import "./vendor/components/ui-button/ui-button.js";
import "./vendor/components/ui-tabs/ui-tabs.js";
import "./vendor/components/ui-textbox/ui-textbox.js";
import "./vendor/components/ui-spinner/ui-spinner.js";
import "./vendor/components/ui-scroll/ui-scroll.js";

// first-party elements
import "./components/ui-tree/ui-tree.js";
import "./components/ui-table/ui-table.js";
import "./components/ui-codeview/ui-codeview.js";

import { apiGet } from "./api.js";
import { register, start } from "./router.js";
import * as home from "./pages/home.js";
import * as peViewer from "./pages/pe-viewer.js";
import * as objdir from "./pages/objdir.js";

register("home", home);
register("pe", peViewer);
register("objdir", objdir);

const content = document.getElementById("content");
start(content);

// version info (nav stagger indices are inlined in index.html —
// post-hoc JS assignment changes animation-delay and restarts finished nav animations)
apiGet("/api/system/info").then((info) => {
  const ver = document.getElementById("ver");
  if (ver) ver.textContent = `NTLab v${info.ntlab} · Win build ${info.build}`;
}).catch(() => {});
