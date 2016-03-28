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

let commandFmt = "{ \"jsonrpc\": \"2.0\", \"id\": 1, \"method\": \"call\", \"params\": [ \"%@\", \"file\", \"write\", { \"path\": \"/sys/class/leds/i-konke:%@/brightness\", \"data\":\"%d\" } ] }" as String

let urlFmt = "http://%@/ubus"

class DeviceDetailsViewController: UIViewController {

    var deviceName: String!
    var deviceIP: String!
    var mySessionID: String!
    var sessionObject: AnyObject?
    
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
        
    }
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    func performPost(url: String, withBody postString: String, funcParam: (NSData, ()->())-> (), callBack: ()->()) {
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
            funcParam(data!, callBack)
        }
        task.resume()
    }
    
    func getSessionID(callBack: () -> ()) {
        let cmd = String( format: getSessionIDFmt, authUserName, authPassword)
        let url = String( format: urlFmt, deviceIP!)
        let handler = getSessionIDHandler
        performPost(url, withBody: cmd, funcParam: handler, callBack: callBack)
    }
    
    func getSessionIDHandler(data: NSData, callBack: ()->()) {
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
            callBack()
        } catch {
            print("Error parsing json")
        }
    }
    
    @IBAction func relayAction(sender: AnyObject) {
        let handler = relayActionHandler
        getSessionID(handler)
    }

    func relayActionHandler() {
        // Handle USB switch
        print("My session id is \(mySessionID)")
    }

    @IBAction func usbAction(sender: AnyObject) {
        let handler = usbActionHandler
        getSessionID(handler)
    }
    
    func usbActionHandler() {
        // Handle USB switch
        print("My session id is \(mySessionID)")
    }
    
    @IBAction func ledAction(sender: AnyObject) {
        let handler = ledActionHandler
        getSessionID(handler)
    }
 
    func ledActionHandler() {
        // Handle USB switch
        print("My session id is \(mySessionID)")
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
