const puppeteer = require('puppeteer');
const fs = require('fs');

// Object to store third-party cookies from all sites
let thirdPartyCookies = {};

async function analyzeSite(url, browser) {
    console.log(`Analyzing: ${url}`);
    const page = await browser.newPage();

    // Intercept response to capture cookies set in HTTP headers
    page.on('response', async response => {
        const requestUrl = new URL(response.url());
        const domain = requestUrl.hostname.replace('www.', '');
        if (!url.includes(domain)) { //'url' is the domain of the current site being analyzed
            try {
                const responseHeaders = response.headers();
                if (responseHeaders['set-cookie']) {
                    // Parse and store third-party cookies
                    if (!thirdPartyCookies[domain]) {
                        thirdPartyCookies[domain] = [];
                    }
                    thirdPartyCookies[domain].push({
                        url: url,
                        cookies: responseHeaders['set-cookie']
                    });
                }
            } catch (error) {
                console.error(`Error capturing cookies from ${response.url()}: `, error);
            }
        }
    });

    await page.goto(url, { waitUntil: 'networkidle2', timeout: 60000 });
    await page.close();
}

(async () => {
    const browser = await puppeteer.launch({
    headless: true, // true for headless
    args: ['--disable-http2'] //disable HTTP/2
  });
    const urls = [
    'https://google.com',
'https://youtube.com',
'https://facebook.com',
'https://instagram.com',
'https://x.com',
'https://baidu.com',
'https://wikipedia.org',
'https://yahoo.com',
'https://whatsapp.com',
'https://amazon.com',
'https://tiktok.com',
'https://reddit.com',
'https://live.com',
'https://linkedin.com',
'https://netflix.com',
'https://openai.com',
'https://office.com',
'https://bing.com',
'https://samsung.com',
'https://vk.com',
'https://pinterest.com',
'https://turbopages.org',
'https://discord.com',
'https://weather.com',
'https://microsoft.com',
'https://max.com',
'https://twitch.tv',
'https://t.me',
'https://quora.com',
'https://qq.com',
'https://duckduckgo.com',
'https://fandom.com',
'https://zoom.us',
'https://ebay.com',
'https://sharepoint.com',
'https://cnn.com',
'https://bbc.co.uk',
'https://nytimes.com',
'https://forbes.com',
'https://theguardian.com',
'https://huffpost.com',
'https://buzzfeed.com',
'https://usatoday.com',
'https://wsj.com',
'https://reuters.com',
'https://nbcnews.com',
'https://cbsnews.com',
'https://news.yahoo.com',
'https://foxnews.com',
'https://time.com',
'https://theatlantic.com',
'https://politico.com',
'https://newsweek.com',
'https://slate.com',
'https://dailybeast.com',
'https://vice.com',
'https://techcrunch.com',
'https://engadget.com',
'https://wired.com',
'https://theverge.com',
'https://cnet.com',
'https://gizmodo.com',
'https://mashable.com',
'https://zdnet.com',
'https://arstechnica.com',
'https://digitaltrends.com',
'https://9to5mac.com',
'https://thenextweb.com',
'https://venturebeat.com',
'https://kotaku.com',
'https://ign.com',
'https://gameinformer.com',
'https://gamespot.com',
'https://polygon.com',
'https://eurogamer.net',
'https://destructoid.com',
'https://gamasutra.com',
'https://gamefaqs.com',
'https://steamcommunity.com',
'https://gamesradar.com',
'https://pcgamer.com',
'https://giantbomb.com',
'https://metacritic.com',
'https://rockpapershotgun.com',
    ];

    for (let url of urls) {
        await analyzeSite(url, browser);
    }

    await browser.close();

    // Save or process the captured third-party cookies data
    fs.writeFileSync('thirdPartyCookies.json', JSON.stringify(thirdPartyCookies, null, 2));
    console.log('Analysis complete. Data saved to thirdPartyCookies.json');

    // Next steps: Analyze 'thirdPartyCookies.json' to understand tracking
})();
