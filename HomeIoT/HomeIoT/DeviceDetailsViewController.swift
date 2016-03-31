//
//  DeviceDetailsViewController.swift
//  HomeIoT
//
//  Created by Edwin Tam on 28/3/16.
//  Copyright © 2016 Edwin Tam. All rights reserved.
//

import UIKit

/* flow

{ "jsonrpc": "2.0", "id": 1, "method": "call", "params": [ "00000000000000000000000000000000", "session", "login", { "username": "root", "password": "Akihabara"  } ] }

*/

let getSessionIDFmt = "{ \"jsonrpc\": \"2.0\", \"id\": 1, \"method\": \"call\", \"params\": [ \"00000000000000000000000000000000\", \"session\", \"login\", { \"username\": \"%@\", \"password\": \"%@\"  } ] }" as String

let commandFmt = "{ \"jsonrpc\": \"2.0\", \"id\": 1, \"method\": \"call\", \"params\": [ \"%@\", \"file\", \"%@\", { \"path\": \"/sys/class/leds/i-konke:%@/brightness\", \"data\":\"%d\" } ] }" as String

let powerFmt = "{ \"jsonrpc\": \"2.0\", \"id\": 1, \"method\": \"call\", \"params\": [ \"%@\", \"file\", \"read\", { \"path\": \"/tmp/power\", \"data\":\"%d\" } ] }" as String

let urlFmt = "http://%@/ubus"

class DeviceDetailsViewController: UIViewController {

    var timer: NSTimer!
    var deviceName: String!
    var deviceIP: String!
    var mySessionID: String!
    var sessionObject: AnyObject?
    var deviceDict: [String: String] = ["Light": "yellow:light", "USB": "green:usb", "Relay": "red:relay", "Power": "/tmp/power"]

    
    var authUserName = "root"
    var authPassword = "Akihabara"

    @IBOutlet weak var uiDeviceName: UILabel!
    @IBOutlet weak var uiDeviceIP: UILabel!
    @IBOutlet weak var uiPower: UILabel!
    @IBOutlet weak var uiPowerBar: UISlider!
    @IBOutlet weak var uiLEDSwitch: UISwitch!
    @IBOutlet weak var uiUSBSwitch: UISwitch!
    @IBOutlet weak var uiRelaySwitch: UISwitch!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view.
    }

    override func viewWillAppear(animated: Bool) {
        uiDeviceName.text = deviceName
        uiDeviceIP.text = deviceIP
        self.relayQuery(uiRelaySwitch)
        self.usbQuery(uiUSBSwitch)
        self.ledQuery(uiLEDSwitch)
        self.powerQuery(uiPowerBar)
        self.timer = NSTimer.scheduledTimerWithTimeInterval(5, target: self, selector: #selector(self.update), userInfo: nil, repeats: true)
    }
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    override func viewWillDisappear(animated: Bool) {
        self.timer.invalidate()
    }
    
    func update() {
        self.powerQuery(uiPowerBar)
    }
    
    
    // ******* ///
    func performPost(sender: AnyObject, url: String, withBody postString: String, handler: (sender: AnyObject, NSData, (sender: AnyObject, status: Int)->())-> (), callBack: (sender: AnyObject, status: Int)->()) {
        let request = NSMutableURLRequest(URL: NSURL(string: url)!)
        request.HTTPMethod = "POST"
        request.HTTPBody = postString.dataUsingEncoding(NSUTF8StringEncoding)
        let task = NSURLSession.sharedSession().dataTaskWithRequest(request) { data, response, error in
            guard error == nil && data != nil else {                                                          // check for fundamental networking error
                print("error=\(error)")
                return
            }
            
            if let httpStatus = response as? NSHTTPURLResponse where httpStatus.statusCode != 200 {           // check for http errors
                print("statusCode should be 200, but is \(httpStatus.statusCode)")
                print("response = \(response)")
            }
            
//            let responseString = NSString(data: data!, encoding: NSUTF8StringEncoding)!
            handler(sender: sender, data!, callBack)
        }
        task.resume()
    }
    
    func getSessionID(sender: AnyObject, callBack: (sender: AnyObject, status: Int) -> ()) {
        let cmd = String( format: getSessionIDFmt, authUserName, authPassword)
        let url = String( format: urlFmt, deviceIP!)
        let handler = getSessionIDHandler
        performPost(sender, url: url, withBody: cmd, handler: handler, callBack: callBack)
    }
    
    func getSessionIDHandler(sender: AnyObject, data: NSData, callBack: (sender: AnyObject, status: Int)->()) {
        var sessionID: String!
        do {
            let json = try NSJSONSerialization.JSONObjectWithData(data, options: .AllowFragments)
            guard let object = json as? [String: AnyObject]
                 else {
                    return
            }
            guard let result = object["result"] as? NSArray else {
                return
            }
//            let sessionJSON = result[1]
            let sessionDict = result[1]
            sessionID = sessionDict["ubus_rpc_session"] as? String
            print("Found session id \(sessionID)")
            mySessionID = sessionID
            callBack(sender: sender, status: 1)
        } catch {
            print("Error parsing json")
            callBack(sender: sender, status: 0)
        }
    }
    
    func sendCommand(sender: AnyObject, device: String, read: Int, onOff: Int, callBack: (sender: AnyObject, status: Int) -> ()) {
        if let phyDevice = deviceDict[device] {
            var rw: String! = "read"
            if (read == 0) {
                rw = "write"
            }
            var cmd = String( format: commandFmt, mySessionID, rw, phyDevice, onOff)
            if (read == 2) {
                cmd = String( format: powerFmt, mySessionID, onOff)
            }
            let url = String( format: urlFmt, deviceIP!)
            let handler = sendCommandHandler
            performPost(sender, url: url, withBody: cmd, handler: handler, callBack: callBack)
        }
    }
    
    func sendCommandHandler(sender: AnyObject, data: NSData, callBack: (sender: AnyObject, status: Int)->()) {
        var status = -1
        do {
            let json = try NSJSONSerialization.JSONObjectWithData(data, options: .AllowFragments)
            guard let object = json as? [String: AnyObject]
                else {
                    return
            }
            guard let result = object["result"] as? NSArray else {
                return
            }
            status = 1
            if (result.count != 1) {
                // This is a Read result
                let resultDict = result[1]
                let recvdata = resultDict["data"] as! String
                print("Received data: \(recvdata)")
                let s = recvdata.stringByTrimmingCharactersInSet(NSCharacterSet.newlineCharacterSet())
                if (s.characters.contains(" ")) {
                    // this is a power ouput
                    let powerValues = s.componentsSeparatedByString(" ")
                    let powerValue = powerValues[1]
                    let powerf = Float(powerValue) ?? 0.0
                    status = Int(powerf*100)
                } else {
                    status = Int(s) ?? 0
                }
                print ("Status is \(status)")
                
            } else {
                // This is a write result
                if ((result[0] as! Int) == 2) {
                    status = 1
                }
            }
            callBack(sender: sender, status: status)
        } catch {
            print("Error parsing json")
            callBack(sender: sender, status: status)
        }
       
    }
    
    @IBAction func relayAction(sender: AnyObject) {
        let handler = relayActionHandler
        getSessionID(sender, callBack: handler)
    }

    func relayActionHandler(sender: AnyObject, status: Int) {
        // Handle Relay switch
        print("My session id is \(mySessionID)")
        let callBackHandler = relayStatusHandler
        if let whoever = sender as? UISwitch {
            var onOff = 0
            if (whoever.on) {
                onOff = 1
            }
            sendCommand(sender, device: "Relay", read: 0, onOff: onOff, callBack: callBackHandler)
        }
    }

    func relayStatusHandler(sender: AnyObject, status: Int) {
        let whoever = sender as! UISwitch
        if (status == -1) {
            dispatch_async(dispatch_get_main_queue()) {
                whoever.setOn(!whoever.on, animated: true)
                whoever.setNeedsDisplay()
            }
        }
    }
    
    func relayQuery(sender: AnyObject) {
        let handler = relayQueryHandler
        getSessionID(sender, callBack: handler)
    }
    
    func relayQueryHandler(sender: AnyObject, status: Int) {
        // Handle Relay query
        print("My session id is \(mySessionID)")
        let callBackHandler = relayQueryStatusHandler
        sendCommand(sender, device: "Relay", read: 1, onOff: 0, callBack: callBackHandler)
    }
    
    func relayQueryStatusHandler(sender: AnyObject, status: Int) {
        let whoever = sender as! UISwitch
        if (status != -1) {
            dispatch_async(dispatch_get_main_queue()) {
                whoever.setOn((status == 1), animated: false)
                whoever.setNeedsDisplay()
            }
        }
    }

    
    
    @IBAction func usbAction(sender: AnyObject) {
        let handler = usbActionHandler
        getSessionID(sender, callBack: handler)
    }
    
    func usbActionHandler(sender: AnyObject, status: Int) {
        // Handle USB switch
        print("My session id is \(mySessionID)")
        let callBackHandler = usbStatusHandler
        if let whoever = sender as? UISwitch {
            var onOff = 0
            if (whoever.on) {
                onOff = 1
            }
            sendCommand(sender, device: "USB", read: 0, onOff: onOff, callBack: callBackHandler)
        }
    }
    
    func usbStatusHandler(sender: AnyObject, status: Int) {
        let whoever = sender as! UISwitch
        if (status == -1) {
            dispatch_async(dispatch_get_main_queue()) {
                whoever.setOn(!whoever.on, animated: true)
                whoever.setNeedsDisplay()
            }
        }
    }

    func usbQuery(sender: AnyObject) {
        let handler = usbQueryHandler
        getSessionID(sender, callBack: handler)
    }
    
    func usbQueryHandler(sender: AnyObject, status: Int) {
        // Handle Usb query
        print("My session id is \(mySessionID)")
        let callBackHandler = usbQueryStatusHandler
        sendCommand(sender, device: "USB", read: 1, onOff: 0, callBack: callBackHandler)
    }
    
    func usbQueryStatusHandler(sender: AnyObject, status: Int) {
        let whoever = sender as! UISwitch
        if (status != -1) {
            dispatch_async(dispatch_get_main_queue()) {
                whoever.setOn((status == 1), animated: false)
                whoever.setNeedsDisplay()
            }
        }
    }

    
    @IBAction func ledAction(sender: AnyObject) {
        let handler = ledActionHandler
        getSessionID(sender, callBack: handler)
    }
 
    func ledActionHandler(sender: AnyObject, status: Int) {
        // Handle LED switch
        print("My session id is \(mySessionID)")
        let callBackHandler = ledStatusHandler
        if let whoever = sender as? UISwitch {
            var onOff = 0
            if (whoever.on) {
                onOff = 1
            }
            sendCommand(sender, device: "Light", read: 0, onOff: onOff, callBack: callBackHandler)
        }
    }

    func ledStatusHandler(sender: AnyObject, status: Int) {
        let whoever = sender as! UISwitch
        if (status == -1) {
            dispatch_async(dispatch_get_main_queue()) {
                whoever.setOn(!whoever.on, animated: true)
                whoever.setNeedsDisplay()
            }
        }
    }


    func ledQuery(sender: AnyObject) {
        let handler = ledQueryHandler
        getSessionID(sender, callBack: handler)
    }
    
    func ledQueryHandler(sender: AnyObject, status: Int) {
        // Handle LED query
        print("My session id is \(mySessionID)")
        let callBackHandler = ledQueryStatusHandler
        sendCommand(sender, device: "Light", read: 1, onOff: 0, callBack: callBackHandler)
    }
    
    func ledQueryStatusHandler(sender: AnyObject, status: Int) {
        let whoever = sender as! UISwitch
        if (status != -1) {
            dispatch_async(dispatch_get_main_queue()) {
                whoever.setOn((status == 1), animated: false)
                whoever.setNeedsDisplay()
            }
        }
    }
    
    func powerQuery(sender: AnyObject) {
        let handler = powerQueryHandler
        getSessionID(sender, callBack: handler)
    }
    
    func powerQueryHandler(sender: AnyObject, status: Int) {
        // Handle LED query
        print("My session id is \(mySessionID)")
        let callBackHandler = powerQueryStatusHandler
        sendCommand(sender, device: "Power", read: 2, onOff: 0, callBack: callBackHandler)
    }
    
    func powerQueryStatusHandler(sender: AnyObject, status: Int) {
        let whoever = sender as! UISlider
        if (status != -1) {
            dispatch_async(dispatch_get_main_queue()) {
                let v: Float = Float(status)/100.0
                whoever.setValue(v, animated: true)
                self.uiPower.text = String(v)
                whoever.setNeedsDisplay()
            }
        }
    }


    /*
    // MARK: - Navigation

    // In a storyboard-based application, you will often want to do a little preparation before navigation
    override func prepareForSegue(segue: UIStoryboardSegue, sender: AnyObject?) {
        // Get the new view controller using segue.destinationViewController.
        // Pass the selected object to the new view controller.
    }
    */

}
