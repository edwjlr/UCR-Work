// popup.js
chrome.tabs.query({active: true, currentWindow: true}, function(tabs) {
    const currentTab = tabs[0];
    if (currentTab && currentTab.url) {
        const url = new URL(currentTab.url);
        const site = url.hostname;
        document.getElementById('siteName').innerText = `Cookies for: ${site}`;

        chrome.runtime.sendMessage({action: "getCookiesForSite", site: site}, (response) => {
            let cookieList = document.getElementById('cookieList');
            cookieList.innerHTML = ''; // Clear current list
            response.cookies.forEach(function(cookie) {
                cookieList.innerHTML += `<li>${cookie.name}: ${cookie.value}</li>`;
            });
        });
    }
});

function getCookiesForSiteFromStorage(site, callback) {
  chrome.storage.local.get({cookiesBySite: {}}, function(data) {
    const cookiesForSite = data.cookiesBySite[site] || {};
    callback(cookiesForSite);
  });
}

// Example usage in popup.js
document.addEventListener('DOMContentLoaded', function() {
    chrome.tabs.query({active: true, currentWindow: true}, function(tabs) {
        const currentTab = tabs[0];
        if (currentTab && currentTab.url) {
            const url = new URL(currentTab.url);
            const site = url.hostname;
            document.getElementById('siteName').innerText = `Cookies for: ${site}`;

            chrome.runtime.sendMessage({action: "getCookiesForSite", site: site}, (response) => {
                let cookieList = document.getElementById('cookieList');
                cookieList.innerHTML = ''; // Clear current list
                
                response.cookies.forEach(function(cookie) {
                    // Create a list item for each cookie
                    let li = document.createElement('li');
                    li.innerHTML = `<strong>Name:</strong> ${cookie.name} <br>
                                    <strong>Value:</strong> ${cookie.value} <br>
                                    <strong>Domain:</strong> ${cookie.domain} <br>
                                    <strong>Path:</strong> ${cookie.path} <br>
                                    <strong>Expiration:</strong> ${new Date(cookie.expirationDate * 1000).toLocaleString()} <br>
                                    <strong>Secure:</strong> ${cookie.secure} <br>
                                    <strong>HttpOnly:</strong> ${cookie.httpOnly} <br>
                                    <strong>SameSite:</strong> ${cookie.sameSite}`;
                    cookieList.appendChild(li);
                });
            });
        }
    });
});
