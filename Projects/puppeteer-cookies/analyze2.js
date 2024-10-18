const fs = require('fs');

// Load the third-party cookies data
const data = JSON.parse(fs.readFileSync('thirdPartyCookies.json', 'utf-8'));

// Object to store occurrences of cookie values
let cookieValues = {};

Object.keys(data).forEach(domain => {
    data[domain].forEach(entry => {
        // Handling the case where cookies are a single string containing multiple cookie definitions
        // Splitting by semicolon and newline to get individual cookies
        const cookiesArray = entry.cookies.split(/;\s*|\n+/);
        cookiesArray.forEach(cookieHeader => {
            const cookieParts = cookieHeader.split('='); // Splitting cookie into name and value
            if (cookieParts.length > 1) { // Ensuring the split resulted in at least a name and value
                const cookieName = cookieParts[0].trim();
                const cookieValue = cookieParts.slice(1).join('=').trim(); // Handling cookie values containing '='
                
                // Tracking cookie values, associating them with domains
                if (!cookieValues[cookieValue]) {
                    cookieValues[cookieValue] = [domain];
                } else if (!cookieValues[cookieValue].includes(domain)) {
                    cookieValues[cookieValue].push(domain);
                }
            }
        });
    });
});

// Identify common cookie values used across domains
let commonCookieValues = Object.entries(cookieValues).filter(([, domains]) => domains.length > 1);

console.log('Common Cookie IDs across websites:', commonCookieValues);
