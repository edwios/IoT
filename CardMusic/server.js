var express = require('express')
var osascript = require('node-osascript')
var app = express()

app.get('/', function (req, res) {
  res.send('Hello World!')
})

app.get('/play', function(req, res, next) {
  var trackID = req.query.trackid
  var trackName = req.query.name
  if (trackID === undefined || trackName === undefined) next('route')
  else next()
}, function (req, res, next) {
  var trackID = req.query.trackid
  var trackName = req.query.name
  res.send("<p>Playing "+trackName+","+trackID+"</p>")
  osascript.execute('tell application "iTunes"\nset fx to fixed indexing\nset fixed indexing to true\nrepeat with thisTrack in (search library playlist 1 for XXX only songs)\nset nn to database ID of thisTrack as text\nif nn is YYY then\nplay thisTrack\nexit repeat\nend if\nend repeat\nset fixed indexing to fx\nend tell\n', { XXX : trackName,YYY : trackID }, function(err, result, raw) {
  if (err) return console.error(err)
//  console.log(result, raw)
  })
})

app.get('/play', function(req, res, next) {
  res.send("<p>Play</p>")
  osascript.execute('tell application "iTunes"\nplay\nend tell', function(err, result, raw) {
  if (err) return console.error(err)
//    console.log(result, raw)
  })
})
 
app.get('/stop', function(req, res, next) {
  res.send("<p>Stopped</p>")
  osascript.execute('tell application "iTunes"\nstop\nend tell', function(err, result, raw) {
  if (err) return console.error(err)
//    console.log(result, raw)
  })
})

app.get('/name', function(req, res) {
    osascript.execute('tell application "iTunes"\nreturn the name of the current track\nend tell', function(err, result, raw) {
    res.send(result) })
})

app.get('/trackid', function(req, res) {
    osascript.execute('tell application "iTunes"\nreturn the database id of the current track\nend tell', function(err, result, raw) {
      if (result === undefined) {
        res.send("")
      } else {
        var ss = result.toString();
        res.send(ss )
      }
    })
})

var server = app.listen(8888, '10.0.1.253', function () {

  var host = server.address().address
  var port = server.address().port

  console.log('Example app listening at http://%s:%s', host, port)

})
