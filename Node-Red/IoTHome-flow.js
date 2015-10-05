[{"id":"9eac092b.6153f8","type":"mqtt-broker","broker":"10.0.1.250","port":"18
[{"83","clientid":"NodeRed"},{"id":"9a4340f9.65bcc","type":"mqtt
[{"out","name":"Set Light Color","topic":"/sensornet/command/color","qos":"","
[{"retain":"","broker":"9eac092b.6153f8","x":2066.500030517578,"y":487.0000381
[{"4697266,"z":"4d31febf.b2ce","wires":[]},{"id":"cbcc447a.3433b8","type":"inj
[{"ect","name":"Full Brightness","topic":"/sensornet/command/color","payload":
[{""{\"Command\":\"COLOR\",\"Color\":[255,255,255]}","payloadType":"string","r
[{"epeat":"","crontab":"","once":false,"x":166.75000762939453,"y":720.25000619
[{"8883,"z":"4d31febf.b2ce","wires":[["9a4340f9.65bcc"]]},{"id":"8418dab1.7be7
[{"28","type":"inject","name":"Off","topic":"/sensornet/command/color","payloa
[{"d":"{\"Command\":\"COLOR\",\"Color\":[0,0,0]}","payloadType":"string","repe
[{"at":"","crontab":"","once":false,"x":184.75000762939453,"y":779.25000619888
[{"3,"z":"4d31febf.b2ce","wires":[["9a4340f9.65bcc"]]},{"id":"8a616939.759e98"
[{","type":"inject","name":"Blue","topic":"/sensornet/command/color","payload"
[{":"{\"Command\":\"COLOR\",\"Color\":[0,0,255]}","payloadType":"string","repe
[{"at":"","crontab":"","once":false,"x":188.75000762939453,"y":543.25000619888
[{"3,"z":"4d31febf.b2ce","wires":[["9a4340f9.65bcc"]]},{"id":"2005b3be.dffa4c"
[{","type":"inject","name":"Warm white","topic":"/sensornet/command/color","pa
[{"yload":"{\"Command\":\"COLOR\",\"Color\":[200,200,80]}","payloadType":"stri
[{"ng","repeat":"","crontab":"","once":false,"x":175.75000762939453,"y":606.25
[{"0006198883,"z":"4d31febf.b2ce","wires":[["9a4340f9.65bcc"]]},{"id":"4638d44
[{"9.b9c72c","type":"inject","name":"Pink","topic":"/sensornet/command/color",
[{""payload":"{\"Command\":\"COLOR\",\"Color\":[255,40,40]}","payloadType":"st
[{"ring","repeat":"","crontab":"","once":false,"x":187.75000762939453,"y":667.
[{"250006198883,"z":"4d31febf.b2ce","wires":[["9a4340f9.65bcc"]]},{"id":"a5601
[{"d40.5a9fe","type":"function","name":"FalseColor","func":"/* accepts
[{"parameters\n * h  Object = {h:x, s:y, v:z}\n * OR \n * h, s,
[{"v\n*/\nfunction HSVtoRGB(h, s, v) {\n    var r, g, b, i, f, p, q, t;\n
[{"if (arguments.length === 1) {\n        s = h.s, v = h.v, h = h.h;\n    }\n
[{"i = Math.floor(h * 6);\n    f = h * 6 - i;\n    p = v * (1 - s);\n    q = v
[{"* (1 - f * s);\n    t = v * (1 - (1 - f) * s);\n    switch (i % 6) {\n
[{"case 0: r = v, g = t, b = p; break;\n        case 1: r = q, g = v, b = p;
[{"break;\n        case 2: r = p, g = v, b = t; break;\n        case 3: r = p,
[{"g = q, b = v; break;\n        case 4: r = t, g = p, b = v; break;\n
[{"case 5: r = v, g = p, b = q; break;\n    }\n    return {\n        r:
[{"Math.round(r * 255),\n        g: Math.round(g * 255),\n        b:
[{"Math.round(b * 255)\n    };\n}\n\nvar outputMsgs = [];\nvar tem =
[{"msg.payload;\nvar aTem = (1-tem)-0.1667;\nif (aTem < 0) aTem = 1 +
[{"aTem;\nvar rgb = HSVtoRGB(aTem, 1, 1);\noutputMsgs.push({payload:rgb})\nout
[{"putMsgs.push({payload:aTem})\nreturn outputMsgs;","outputs":"2","noerr":0,"
[{"x":1434.0000190734863,"y":805.2500143051147,"z":"4d31febf.b2ce","wires":[["
[{"80efe594.7f1018"],["df84b785.207b48"]]},{"id":"700889f4.8ff778","type":"ran
[{"ge","minin":"15","maxin":"30","minout":"0","maxout":"1","action":"clamp","r
[{"ound":false,"name":"Scale temperature","x":636.5833282470703,"y":1868.49994
[{"8501587,"z":"4d31febf.b2ce","wires":[["a5601d40.5a9fe"]]},{"id":"80efe594.7
[{"f1018","type":"template","name":"Rewrite JSON msg","field":"payload","forma
[{"t":"handlebars","template":"{\"Command\":\"COLOR\",\"Color\":[{{payload.r}}
[{",{{payload.g}},{{payload.b}}]}\n","x":1692.7500190734863,"y":872.7500143051
[{"147,"z":"4d31febf.b2ce","wires":[["9a4340f9.65bcc"]]},{"id":"df84b785.207b4
[{"8","type":"debug","name":"","active":false,"console":"false","complete":"fa
[{"lse","x":1634.2500190734863,"y":951.0000143051147,"z":"4d31febf.b2ce","wire
[{"s":[]},{"id":"96cd420e.6932c","type":"inject","name":"Green","topic":"/sens
[{"ornet/command/color","payload":"{\"Command\":\"COLOR\",\"Color\":[0,255,0]}
[{"","payloadType":"string","repeat":"","crontab":"","once":false,"x":165.0763
[{"0920410156,"y":466.7175278663635,"z":"4d31febf.b2ce","wires":[["9a4340f9.65
[{"bcc"]]},{"id":"289da2d4.d7625e","type":"inject","name":"Orange","topic":"/s
[{"ensornet/command/color","payload":"{\"Command\":\"COLOR\",\"Color\":[255,80
[{",20]}","payloadType":"string","repeat":"","crontab":"","once":false,"x":161
[{".20498275756836,"y":402.12637758255005,"z":"4d31febf.b2ce","wires":[["9a434
[{"0f9.65bcc"]]},{"id":"71841ade.8e7be4","type":"mqtt in","name":"Balcony temp
[{"","topic":"sensornet/env/home/balcony/temperature","broker":"9eac092b.6153f
[{"8","x":148.5,"y":1823.6664600372314,"z":"4d31febf.b2ce","wires":[["700889f4
[{".8ff778","8235c26f.7dca4","fd36164d.02c9e8"]]},{"id":"e8fb38f9.1704c8","typ
[{"e":"mqtt in","name":"Balcony humi","topic":"sensornet/env/home/balcony/humi
[{"dity","broker":"9eac092b.6153f8","x":155.50000381469727,"y":1894.6665382385
[{"254,"z":"4d31febf.b2ce","wires":[["b5de4fc7.4a21b"]]},{"id":"dd147d49.22eb8
[{"","type":"http request","name":"Thingspeak update","method":"GET","ret":"tx
[{"t","url":"https://api.thingspeak.com/update?key=APIKEY&field1={{f
[{"ield1}}&field2={{field2}}&field3={{field3}}","x":636.4166717529297,"y":1752
[{".8332824707031,"z":"4d31febf.b2ce","wires":[[]]},{"id":"423d605d.bdc2a","ty
[{"pe":"debug","name":"msg","active":true,"console":"false","complete":"payloa
[{"d","x":872.8333435058594,"y":1305.750051498413,"z":"4d31febf.b2ce","wires":
[{"[]},{"id":"a7a329b3.585cd8","type":"mqtt out","name":"Dump","topic":"sensor
[{"net/command/dump","qos":"0","retain":"false","broker":"9eac092b.6153f8","x"
[{":425.7142982482908,"y":1120.5715312957764,"z":"4d31febf.b2ce","wires":[]},{
[{""id":"afe55072.501ab","type":"inject","name":"Dump","topic":"sensornet/comm
[{"and/dump","payload":"dump","payloadType":"string","repeat":"","crontab":"",
[{""once":false,"x":157.71429824829102,"y":1122.5715312957764,"z":"4d31febf.b2
[{"ce","wires":[["a7a329b3.585cd8"]]},{"id":"8235c26f.7dca4","type":"function"
[{","name":"Context
[{"var","func":"context.global.balcony_temp=msg.payload\nreturn msg;","outputs
[{"":1,"noerr":0,"x":352.50000762939453,"y":1825.8332042694092,"z":"4d31febf.b
[{"2ce","wires":[[]]},{"id":"b5de4fc7.4a21b","type":"function","name":"Context
[{"var","func":"context.global.balcony_humi=msg.payload\nreturn msg;","outputs
[{"":1,"noerr":0,"x":351.2499694824219,"y":1896.666540145874,"z":"4d31febf.b2c
[{"e","wires":[[]]},{"id":"fd36164d.02c9e8","type":"function","name":"Thingspe
[{"ak data","func":"msg.field1=context.global.balcony_temp\nmsg.field2=context
[{".global.balcony_humi\nmsg.field3=context.global.balcony_brgt\nreturn msg;",
[{""outputs":1,"noerr":0,"x":413.75000381469727,"y":1754.5831985473633,"z":"4d
[{"31febf.b2ce","wires":[["dd147d49.22eb8"]]},{"id":"c33ca296.3cc36","type":"m
[{"qtt in","name":"Balcony bright","topic":"sensornet/env/home/balcony/brightn
[{"ess","broker":"9eac092b.6153f8","x":152.50000381469727,"y":1968.33319854736
[{"33,"z":"4d31febf.b2ce","wires":[["91711c1a.6e8ee"]]},{"id":"91711c1a.6e8ee"
[{","type":"function","name":"Context
[{"var","func":"context.global.balcony_brgt=msg.payload\nreturn msg;","outputs
[{"":1,"noerr":0,"x":354.9166564941406,"y":1968.66654586792,"z":"4d31febf.b2ce
[{"","wires":[[]]},{"id":"9a448261.65bb8","type":"mqtt in","name":"Living Room
[{"Motion","topic":"sensornet/env/home/living/motion","broker":"9eac092b.6153f
[{"8","x":165.83330535888672,"y":1544.9999542236328,"z":"4d31febf.b2ce","wires
[{"":[["93b19a3.f6c4e68"]]},{"id":"93b19a3.f6c4e68","type":"function","name":"
[{"Context var","func":"context.global.living_motion=msg.payload\nreturn msg;"
[{","outputs":1,"noerr":0,"x":396.58329010009766,"y":1543.6665649414062,"z":"4
[{"d31febf.b2ce","wires":[[]]},{"id":"43cb7d67.bc3484","type":"mqtt
[{"in","name":"Living Room Sound","topic":"sensornet/env/home/living/sound","b
[{"roker":"9eac092b.6153f8","x":166.24998474121094,"y":1619.1666412353516,"z":
[{""4d31febf.b2ce","wires":[["a5c46db5.5a3b9"]]},{"id":"a5c46db5.5a3b9","type"
[{":"function","name":"Context
[{"var","func":"context.global.living_sound=msg.payload\nreturn msg;","outputs
[{"":1,"noerr":0,"x":395.3333282470703,"y":1614.4999418258667,"z":"4d31febf.b2
[{"ce","wires":[[]]},{"id":"e4de15fa.1b21e8","type":"http
[{"request","name":"Thingspeak update","method":"GET","ret":"txt","url":"https
[{"://api.thingspeak.com/update?key=APIKEY&field1={{{field1}}}&field
[{"2={{{field2}}}&field3={{{field3}}}&field4={{{field4}}}&field5={{{field5}}}"
[{","x":648.3333129882812,"y":1396.6666259765625,"z":"4d31febf.b2ce","wires":[
[{"["423d605d.bdc2a"]]},{"id":"38fee16d.c7011e","type":"function","name":"Thin
[{"gspeak data","func":"msg.field1=context.global.living_temp\nmsg.field2=cont
[{"ext.global.living_motion\nmsg.field3=context.global.living_sound\nmsg.field
[{"4=context.global.living_humi\nmsg.field5=context.global.living_brgt\nmsg.In
[{"doorAPIKey=\"APIKEY\"\nreturn msg;","outputs":1,"noerr":0,"x":410
[{".66666412353516,"y":1281.7499709129333,"z":"4d31febf.b2ce","wires":[["e4de1
[{"5fa.1b21e8"]]},{"id":"301bd21a.cfe42e","type":"mqtt in","name":"LivingRoom 
[{"temp","topic":"sensornet/env/home/living/temperature","broker":"9eac092b.61
[{"53f8","x":165,"y":1351.6666793823242,"z":"4d31febf.b2ce","wires":[["38fee16
[{"d.c7011e","5ef67156.a1099"]]},{"id":"1d484bd3.e2b7b4","type":"mqtt
[{"in","name":"LivingRoom humi","topic":"sensornet/env/home/living/humidity","
[{"broker":"9eac092b.6153f8","x":166.66666412353516,"y":1411.6666812896729,"z"
[{":"4d31febf.b2ce","wires":[["284f0069.d7b1"]]},{"id":"522c4010.add3c","type"
[{":"mqtt in","name":"LivingRoom bright","topic":"sensornet/env/home/living/br
[{"ightness","broker":"9eac092b.6153f8","x":165,"y":1471.6666831970215,"z":"4d
[{"31febf.b2ce","wires":[["6127b00.f9ed85"]]},{"id":"5ef67156.a1099","type":"f
[{"unction","name":"Context
[{"var","func":"context.global.living_temp=msg.payload\nreturn msg;","outputs"
[{":1,"noerr":0,"x":408.33329010009766,"y":1355.000015258789,"z":"4d31febf.b2c
[{"e","wires":[[]]},{"id":"284f0069.d7b1","type":"function","name":"Context
[{"var","func":"context.global.living_humi=msg.payload\nreturn msg;","outputs"
[{":1,"noerr":0,"x":401.6666564941406,"y":1418.333251953125,"z":"4d31febf.b2ce
[{"","wires":[[]]},{"id":"6127b00.f9ed85","type":"function","name":"Context
[{"var","func":"context.global.living_brgt=msg.payload\nreturn msg;","outputs"
[{":1,"noerr":0,"x":396.6666564941406,"y":1475,"z":"4d31febf.b2ce","wires":[[]
[{"]},{"id":"7c5dc281.83a23c","type":"prowl","title":"Intruder in Living
[{"Room","priority":0,"name":"Push Alarm","x":583.428581237793,"y":2550.428634
[{"6435547,"z":"4d31febf.b2ce","wires":[]},{"id":"4e4e5b4.fb1b1a4","type":"mqt
[{"t in","name":"Motion Alarm","topic":"sensornet/env/home/living/alarm/motion
[{"","broker":"9eac092b.6153f8","x":121.42858123779297,"y":2552.4286346435547,
[{""z":"4d31febf.b2ce","wires":[["942fe48c.6bd018","97ff85c2.680078"]]},{"id":
[{""942fe48c.6bd018","type":"debug","name":"","active":false,"console":"false"
[{","complete":"false","x":371.42858123779297,"y":2653.4286346435547,"z":"4d31
[{"febf.b2ce","wires":[]},{"id":"878afeff.7875","type":"inject","name":"Arm
[{"All Alarms","topic":"sensornet/command/arm","payload":"arm","payloadType":"
[{"string","repeat":"","crontab":"","once":false,"x":132,"y":2245,"z":"4d31feb
[{"f.b2ce","wires":[["22108783.ddef78"]]},{"id":"22108783.ddef78","type":"mqtt
[{"out","name":"MQTT Send","topic":"","qos":"","retain":"","broker":"9eac092b.
[{"6153f8","x":525.8571472167969,"y":2318.000066757202,"z":"4d31febf.b2ce","wi
[{"res":[]},{"id":"282ad493.d7d52c","type":"mqtt in","name":"Status","topic":"
[{"sensornet/status/#","broker":"9eac092b.6153f8","x":105.42858123779297,"y":2
[{"618.4286346435547,"z":"4d31febf.b2ce","wires":[["942fe48c.6bd018"]]},{"id":
[{""90ada364.6f526","type":"inject","name":"DisArm All","topic":"sensornet/com
[{"mand/disarm","payload":"disarm","payloadType":"string","repeat":"","crontab
[{"":"","once":false,"x":119,"y":2297,"z":"4d31febf.b2ce","wires":[["22108783.
[{"ddef78"]]},{"id":"97ff85c2.680078","type":"change","name":"Alarm
[{"Message","rules":t":"set","p":"payload","to":"Intruder found"}],"action":""
[{","property":"","from":"","to":"","reg":false,"x":347.42858123779297,"y":255
[{"3.4286346435547,"z":"4d31febf.b2ce","wires":[["7c5dc281.83a23c"]]},{"id":"5
[{"5962a62.aa69d4","type":"comment","name":"House alarms","info":"Turn on and
[{"off house alarms and send alarm via push message to the iPhone.\n","x":132.
[{"85713958740234,"y":2174.285852432251,"z":"4d31febf.b2ce","wires":[]},{"id":
[{""ed27f4fe.12d808","type":"comment","name":"Living room
[{"information","info":"Publish the readings of the sensors in the living room
[{"to Thingspeak.","x":395.71427154541016,"y":1225.7141609191895,"z":"4d31febf
[{".b2ce","wires":[]},{"id":"b4b8ae8a.4b475","type":"comment","name":"Balcony
[{"information","info":"Publish the readings of the sensors in the balcony to 
[{"Thingspeak.","x":391.4285888671875,"y":1691.4285888671875,"z":"4d31febf.b2c
[{"e","wires":[]},{"id":"9158b2c.f6ea75","type":"comment","name":"Force to
[{"update","info":"Issue a \"dump\" topic on the MQTT net. Devices that
[{"response to this topic are suppose to update the sensor readings onto the
[{"MQTT net.","x":211.42857142857142,"y":1044.2857142857142,"z":"4d31febf.b2ce
[{"","wires":[]},{"id":"d6be3781.2941c8","type":"comment","name":"Manual Color
[{"Control","info":"Set the color of the RingLED manually.","x":185.7142857142
[{"857,"y":322.85714285714283,"z":"4d31febf.b2ce","wires":[]},{"id":"fe2d4761.
[{"01d2b8","type":"inject","name":"Console 01 Display Off","topic":"sensornet/
[{"command/envconsole01/display/off","payload":"doff","payloadType":"string","
[{"repeat":"","crontab":"","once":false,"x":154.28571428571428,"y":2358.571428
[{"5714284,"z":"4d31febf.b2ce","wires":[["22108783.ddef78"]]},{"id":"a8b43d82.
[{"574bc","type":"inject","name":"Console 01 Display On","topic":"sensornet/co
[{"mmand/envconsole01/display/on","payload":"don","payloadType":"string","repe
[{"at":"","crontab":"","once":false,"x":154.2857208251953,"y":2418.57153320312
[{"5,"z":"4d31febf.b2ce","wires":[["22108783.ddef78"]]}]
