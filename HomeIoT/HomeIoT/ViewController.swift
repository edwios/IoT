//
//  ViewController.swift
//  HomeIoT
//
//  Created by Edwin Tam on 27/3/16.
//  Copyright © 2016 Edwin Tam. All rights reserved.
//

import UIKit
import CoreData
var myDevices = [NSManagedObject]()
var selectedIndexPath: NSIndexPath?
var selectedDeviceName: String!
var selectedDeviceIP: String!

class ViewController: UIViewController, UICollectionViewDataSource, UICollectionViewDelegate {

    @IBOutlet weak var table: UICollectionView!
    @IBAction func myUnwindAction(unwindSegue: UIStoryboardSegue) {
        
    }

    let SegueManageDevices = "ManageDevices"
    let SegueDeviceDetailsViewController = "DeviceDetails"
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view, typically from a nib.
        print("Main view did load")
    }

    
    override func viewWillAppear(animated: Bool) {
        super.viewWillAppear(animated)
        print("Main view will load")
        self.table.reloadData()
        self.table.setNeedsDisplay()
    }
    
    func getDevices() {
        let appDelegate =
        UIApplication.sharedApplication().delegate as! AppDelegate
        
        let managedContext = appDelegate.managedObjectContext
        
        let fetchRequest = NSFetchRequest(entityName: "Device")
        
        do {
            let results =
            try managedContext.executeFetchRequest(fetchRequest)
            myDevices = results as! [NSManagedObject]
            print("Another fetch \(myDevices.count)")
        } catch let error as NSError {
            print("Could not fetch \(error), \(error.userInfo)")
        }
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }

    let reuseIdentifier = "cell" // also enter this string as the cell identifier in the storyboard
    
    
    // MARK: - UICollectionViewDataSource protocol
    
    // tell the collection view how many cells to make
    func collectionView(collectionView: UICollectionView, numberOfItemsInSection section: Int) -> Int {
        self.getDevices()
        print("Collection \(myDevices.count)")
        return myDevices.count
    }
    
    // make a cell for each cell index path
    func collectionView(collectionView: UICollectionView, cellForItemAtIndexPath indexPath: NSIndexPath) -> UICollectionViewCell {
        
        // get a reference to our storyboard cell
        let cell = collectionView.dequeueReusableCellWithReuseIdentifier(reuseIdentifier, forIndexPath: indexPath) as! ViewCell
        let device = myDevices[indexPath.item]
        // Use the outlet in our custom class to get a reference to the UILabel in the cell
        cell.deviceName.text = device.valueForKey("name") as? String
//        cell.backgroundColor = UIColor.lightGrayColor() // make cell more visible in our example project
        
        return cell
    }
    
    
    // MARK: - UICollectionViewDelegate protocol
    
    func collectionView(collectionView: UICollectionView, didSelectItemAtIndexPath indexPath: NSIndexPath) {
        // handle tap events
        let device = myDevices[indexPath.item]
        selectedIndexPath = indexPath
        selectedDeviceName = device.valueForKey("name") as? String
        selectedDeviceIP = device.valueForKey("ipaddr") as? String
        print("You selected cell #\(indexPath.item)!")
        print("Device \(selectedDeviceName) has IP of \(selectedDeviceIP)")
        performSegueWithIdentifier(SegueDeviceDetailsViewController, sender: self)
    }
    
    override func prepareForSegue(segue: UIStoryboardSegue, sender: AnyObject?) {
        if segue.identifier == SegueDeviceDetailsViewController {
                let destinationViewController = segue.destinationViewController as! DeviceDetailsViewController
                destinationViewController.deviceName = selectedDeviceName
                destinationViewController.deviceIP = selectedDeviceIP
        }
    }

}

