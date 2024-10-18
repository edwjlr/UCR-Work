const fs = require('fs');

// Load the third-party cookies data
const data = JSON.parse(fs.readFileSync('thirdPartyCookies.json', 'utf-8'));

// Object to store the connections: Origin URL -> Array of domains where the cookie from this origin was found
let originToDomains = {};

Object.keys(data).forEach(domain => {
    data[domain].forEach(entry => {
        const originUrl = new URL(entry.url).hostname; // Extract hostname from URL to use as origin
        if (!originToDomains[originUrl]) {
            originToDomains[originUrl] = new Set();
        }
        originToDomains[originUrl].add(domain);
    });
});

// Convert Set to Array for JSON compatibility
let output = {};
Object.keys(originToDomains).forEach(key => {
    output[key] = Array.from(originToDomains[key]);
});

// Write to file
fs.writeFileSync('cookieOriginsGraph.json', JSON.stringify(output, null, 2));

console.log('Output saved to cookieOriginsGraph.json');
