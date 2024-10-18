// background.js

let cookiesBySite = {};

chrome.tabs.onActivated.addListener(activeInfo => {
    // Keep track of the currently active tab
    chrome.tabs.get(activeInfo.tabId, function(tab) {
        // Update some storage or variable with the current tab's URL or domain
        const currentTabDomain = new URL(tab.url).hostname;
        // You might store this in chrome.storage.local or a global variable for later reference
    });
});

chrome.webRequest.onBeforeSendHeaders.addListener(
    function(details) {
        let tabId = details.tabId;
        let url = new URL(details.url);
        let domain = url.hostname;

        // Compare this domain with the tab's main domain to determine if it's third-party
        // Note: You need to maintain a mapping of tabIds to their primary domains
        // This is a simplified example; you'd need additional logic here

        console.log(`Detected request to ${domain} from tab ${tabId}`);
    },
    { urls: ["<all_urls>"] },
    ["requestHeaders"]
);

chrome.webRequest.onBeforeRequest.addListener(
    function(details) {
        // Fetch the currently active tab's domain from storage or a global variable
        chrome.tabs.query({active: true, currentWindow: true}, tabs => {
            if (tabs[0] && details.tabId === tabs[0].id) {
                const currentTabDomain = new URL(tabs[0].url).hostname;
                const requestDomain = new URL(details.url).hostname;
                
                // Check if the request domain is different from the current tab's domain (third-party)
                if (requestDomain !== currentTabDomain) {
                    console.log(`Third-party request to ${requestDomain} from ${currentTabDomain}`);
                }
            }
        });
    },
    {urls: ["<all_urls>"]}, // Listen for all web requests
    []
);

chrome.tabs.onUpdated.addListener((tabId, changeInfo, tab) => {
    if (changeInfo.status === 'complete' && tab.url) {
        const url = new URL(tab.url);
        const site = url.hostname;
        cookiesBySite[site] = [];
        chrome.cookies.getAll({url: tab.url}, function(cookies) {
            cookies.forEach(cookie => {
                // Extract detailed information for each cookie
                const cookieDetails = getCookieDetails(cookie);
                cookiesBySite[site].push(cookieDetails);
            });
            // Update the persistent storage with the latest cookies
            updateStorageWithCookies(site, cookiesBySite[site]);
        });
    }
});

chrome.cookies.onChanged.addListener(changeInfo => {
    const cookie = changeInfo.cookie;
    const cookieDetails = getCookieDetails(cookie);
    const site = getSiteFromCookie(cookie);

    if (!cookiesBySite[site]) {
        cookiesBySite[site] = [];
    }

    // log the changed cookie for now
    console.log('Cookie changed:', cookieDetails);
    // Then, update the storage
    updateStorageWithCookies(site, cookiesBySite[site]);
});

function getCookieDetails(cookie) {
    return {
        name: cookie.name,
        value: cookie.value,
        domain: cookie.domain,
        path: cookie.path,
        secure: cookie.secure,
        httpOnly: cookie.httpOnly,
        expirationDate: cookie.expirationDate,
        sameSite: cookie.sameSite
    };
}

function getSiteFromCookie(cookie) {
    // Here you should implement logic to determine the site from the cookie domain
    // This is a simplistic approach; you may need more sophisticated domain parsing
    return cookie.domain.startsWith('.') ? cookie.domain.substring(1) : cookie.domain;
}

function updateStorageWithCookies(site, cookies) {
    chrome.storage.local.set({[site]: cookies}, function() {
        console.log(`Cookies for ${site} updated in storage.`);
    });
}

// Listen for a message from the popup
chrome.runtime.onMessage.addListener((request, sender, sendResponse) => {
    if (request.action === "getCookiesForSite") {
        chrome.storage.local.get([request.site], function(result) {
            const cookies = result[request.site] || [];
            sendResponse({cookies: cookies});
        });
    }
    return true; // Keep the message channel open for asynchronous response
});
