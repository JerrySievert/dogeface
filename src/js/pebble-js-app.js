var first = false;

Pebble.addEventListener("ready",
  function(e) {
    getCurrentPrice();
  }
);


function getCurrentPrice ( ) {
  var req = new XMLHttpRequest();
    req.open('GET',"http://pubapi.cryptsy.com/api.php?method=singlemarketdata&marketid=132", true);
    req.onload = function(e) {
      if (req.readyState == 4 && req.status == 200) {
        if (req.status == 200) {
          var response;
          try {
            response = JSON.parse(req.responseText);
          } catch (err) {
            setTimeout(getCurrentPrice, 5000);

            return;
          }

          var send = { };

          if (response.return && response.return.markets && response.return.markets.DOGE) {
            var satoshis = response.return.markets.DOGE.lasttradeprice;
            send.satoshis = Number(Number(satoshis) * 100000000).toFixed(0) + " Satoshi";
            first = true;
          }

          if (first) {
            setTimeout(getCurrentPrice, 90000);
          } else {
            setTimeout(getCurrentPrice, 5000);
            return;
          }

          Pebble.sendAppMessage(send);
        }
      }
    };

    req.send(null);
}
