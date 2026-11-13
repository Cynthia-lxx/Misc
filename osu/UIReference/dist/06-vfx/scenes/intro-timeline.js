/* SPDX-License-Identifier: MIT
 * Derived from ppy/osu (MIT) — see 00-overview/LICENSE-MIT.txt
 * Timeline ported from osu.Game/Screens/Menu/IntroTriangles.cs:195-280
 * (BeginAbsoluteSequence + BeginDelayedSequence, ending with GameWideFlash).
 * Accepts both legacy motion.json (sequences.introTriangles with step objects)
 * and v2 (sequences.intro.triangles with a numeric beat array).
 * Never throws: a missing sequence degrades to a no-op player so that a single
 * data-shape change cannot abort the whole demo page again.
 */

import { Timeline } from '../../05-motion/timeline.js';
import { gameWideFlash } from '../glow/glow.js';
import { isReducedMotion } from '../particles/particles.js';

const ACTION_CLASSES = {
  'glitch-triangles': 'ui-motion-elastic',
  'logo-stroke': 'ui-motion-ripple',
  'logo-grow': 'ui-motion-elastic',
  'ruleset-icons-1': 'ui-motion-enter',
  'ruleset-icons-2': 'ui-motion-enter',
  'ruleset-icons-3': 'ui-motion-enter',
};

const V2_ACTION_NAMES = [
  'letter-1', 'letter-2', 'letter-3', 'glitch-triangles', 'logo-stroke',
  'ruleset-icons-1', 'ruleset-icons-2', 'ruleset-icons-3', 'logo-grow', 'game-wide-flash',
];

export function resolveIntroSequence(motion) {
  const legacy = motion?.sequences?.introTriangles;
  if (legacy && Array.isArray(legacy.steps) && typeof legacy.steps[0] === 'object') {
    return legacy;
  }

  const v2 = motion?.sequences?.intro?.triangles;
  if (v2 && Array.isArray(v2.steps)) {
    return {
      duration: v2.duration || 3000,
      steps: v2.steps.map((entry, index) => {
        if (typeof entry === 'object' && entry !== null) return entry;
        return {
          at: Number(entry) || 0,
          action: V2_ACTION_NAMES[index] || `step-${index + 1}`,
          params: entry === 2080 ? { delay: v2.logoGrow } : entry === 3000 ? { duration: v2.flash } : undefined,
        };
      }),
    };
  }
  return null;
}

/**
 * @param {HTMLElement} root
 * @param {Object} motion  parsed 05-motion/motion.json
 * @param {{onStep?:Function}} [options]
 */
export function createIntroPlayer(root, motion, options = {}) {
  const sequence = resolveIntroSequence(motion);
  const listeners = new Set();
  if (typeof options.onStep === 'function') listeners.add(options.onStep);

  if (!sequence) {
    console.warn('[intro] motion.json has no intro sequence; intro player disabled');
    return {
      play() { return this; }, cancel() {}, seek() {}, setRate() {},
      onStep(fn) { if (typeof fn === 'function') listeners.add(fn); return () => listeners.delete(fn); },
      setTheme(name) { if (name) root.dataset.theme = name; },
      get currentTime() { return 0; },
      unmount() { listeners.clear(); },
    };
  }

  const skip = new Set(motion.reduceMotion?.skipActions || []);

  const build = () => {
    const timeline = new Timeline({ duration: sequence.duration });
    for (const step of sequence.steps) {
      timeline.at(step.at, () => {
        const reduced = isReducedMotion();
        if (reduced && skip.has(step.action)) return;

        root.dataset.introStep = step.action;
        root.dispatchEvent(new CustomEvent('ui-intro-step', {
          bubbles: true,
          detail: { step, reduced },
        }));
        for (const fn of listeners) fn(step, root);

        const cssClass = ACTION_CLASSES[step.action];
        if (cssClass && !reduced) {
          root.classList.remove(cssClass);
          void root.offsetWidth; // restart animation
          root.classList.add(cssClass);
        }
        if (step.action === 'game-wide-flash') gameWideFlash(document.body, step.params?.duration || 1000);
        if (step.action.startsWith('letter-')) {
          const index = Number(step.action.split('-')[1]) || 1;
          root.dataset.introLetter = String(index);
        }
      });
    }
    return timeline;
  };

  let timeline = build();

  return {
    play(from = 0) { timeline = build(); timeline.play(from); return this; },
    cancel() { timeline.cancel(); root.removeAttribute('data-intro-step'); },
    seek(ms) { timeline.seek(ms); },
    setRate(rate) { timeline.setRate(rate); },
    onStep(fn) { if (typeof fn === 'function') listeners.add(fn); return () => listeners.delete(fn); },
    setTheme(name) { if (name) root.dataset.theme = name; },
    get currentTime() { return timeline.currentTime; },
    unmount() { timeline.cancel(); listeners.clear(); },
  };
}

export function playIntro(root, motion, options) {
  const player = createIntroPlayer(root, motion, options);
  player.play();
  return player;
}

export default createIntroPlayer;
