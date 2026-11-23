//http://192.168.0.232:8080/index.html
const startInterval = setInterval(getStats, 1000);
const countEl = document.getElementById('client--count');
const bytesEl = document.getElementById('client--bytes');
async function getStats() {
try {
    let response = await fetch("/getStats");
    let json;
    if (response.ok) 
    {  
        const data = await response.json();
        countEl.textContent = data.ClientCount;
        bytesEl.textContent = data.BytesSent;
    }
    else 
    {
      alert("Error HTTP: " + response.status);
    }
    } catch (err) {
        clearInterval(startInterval);
        console.warn("Server is unavailable:", err);
    }
}

