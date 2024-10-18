const puppeteer = require('puppeteer');

(async () => {
  const browser = await puppeteer.launch();
  const page = await browser.newPage();

  // Intercept network requests
  await page.setRequestInterception(true);
  page.on('request', (req) => {
    const url = req.url();
    // Example: Detect tracking pixels by size and known URLs
    if (url.includes('trackingpixel') || url.includes('/analytics')) {
      console.log('Tracking Pixel or Analytics Request Detected:', url);
    }
    // Example: Detect session replay or heatmap scripts by URL
    if (url.includes('sessionreplay') || url.includes('/heatmap')) {
      console.log('Session Replay or Heatmap Request Detected:', url);
    }
    req.continue();
  });

  // Evaluate scripts on the page for fingerprinting techniques
  await page.evaluate(() => {
    // This is a simplified example. In a real scenario, you would look for
    // specific scripts or libraries known for fingerprinting.
    const scripts = Array.from(document.scripts).map(s => s.src);
    if (scripts.some(src => src.includes('fingerprintjs'))) {
      console.log('Fingerprinting Script Detected:', src);
    }
  });

  // Listen for DOM modifications (simplified approach)
  page.on('domcontentloaded', async () => {
    const trackingElements = await page.evaluate(() => {
      // Look for hidden images that might be used as tracking pixels
      const imgs = Array.from(document.images).filter(img => img.width <= 1 && img.height <= 1);
      return imgs.map(img => img.src);
    });
    if (trackingElements.length) {
      console.log('Possible Tracking Pixels Detected:', trackingElements);
    }
  });

  // Navigate to the target webpage
  await page.goto('https://buzzfeed.com');

  await browser.close();
})();
