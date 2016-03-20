preferences
{
  input("deviceId", "text", title: "Device ID")
  input("accessToken", "text", title: "Access Token")
}

metadata
{
  definition (name: "Ankit Photon Switch", author: "Ankit Mehta")
  {
    capability "Switch"
    capability "Refresh"
    capability "Polling"
  }
  simulator
  {	}
  tiles
  {
    standardTile("switch", "device.switch", width: 2, height: 2, canChangeIcon: true)
    {
      state "offline", label:"Offline", icon:"st.switches.switch.off", backgroundColor:"#ffffff"
      state "off", label:"Off", action:"switch.on", icon:"st.switches.switch.off", backgroundColor:"#ffffff", nextState:"switchingOn"
      state "switchingOn", label:"Switching On", icon:"st.switches.switch.on", backgroundColor:"#79b821", nextState:"on"
      state "on", label:"On", action:"switch.off", icon:"st.switches.switch.on", backgroundColor:"#79b821", nextState:"switchingOff"
      state "switchingOff", label:"Switching Off", icon:"st.switches.switch.off", backgroundColor:"#ffffff", nextState:"off"
    }
    standardTile("refresh", "device.switch", inactiveLabel: false, decoration: "flat")
    {
      state "default", label:"", action:"refresh.refresh", icon:"st.secondary.refresh"
    }
    main("switch")
    details(["switch", "refresh"])
  }
}

def parse(String description)
{
  log.error "This device does not support incoming events"
  return null
}

def on()
{ 
  execute("ON")
}

def off()
{
  execute("OFF")
}

def poll()
{
  refresh()
}

def refresh()
{
  log.debug("refresh called")
  try
  {
      httpGet(
        uri: "https://api.particle.io/v1/devices/${deviceId}/buttonState?access_token=${accessToken}"
        )
        {
          response -> log.debug(response.data)
          log.debug(response.data.result)
          try
          {
              if(response.data.result == 1)
              {
                  log.debug("It was one")
                  updateStatus("on")
              }
              else if(response.data.result == 0)
              {
                  log.debug("It was zero")
                  updateStatus("off")
              }
              else
              {
                  log.debug("No data found")
                  updateStatus("offline")
              }
          }
          catch(all)
          {
              log.debug("Error during refresh while parsing response")
              updateStatus("offline")
          }
        }
    }
    catch(all)
    {
        log.debug("Error during refresh in catch all for httpget")
        updateStatus("offline")
    }
}

private execute(commandText)
{
  if(commandText == "ON")
  {
    updateStatus("switchingOn")
  }
  else if(commandText == "OFF")
  {
    updateStatus("switchingOff")
  }
  else
  {
    return
  }
  try
  {
      httpPost(
        uri: "https://api.particle.io/v1/devices/${deviceId}/execute",
        body: [access_token: accessToken, args: commandText],
      ) {
           response -> log.debug (response.data)
           log.debug(response.data.return_value)
           if(response.data.return_value == 1) {
               updateStatus("on")
           }
           else
           {
               updateStatus("off")
           }
        }
   }
   catch(all)
   {
       log.debug("Error during execute command")
       refresh()
   }
}

def updateStatus(status)
{
	if (status == "offline") { sendEvent(name: "switch", value: "", display: true, descriptionText: device.displayName + " is offline") }
    if (status == "off") { sendEvent(name: "switch", value: "off", display: true, descriptionText: device.displayName + " was switched off") }
	if (status == "on") { sendEvent(name: "switch", value: "on", display: true, descriptionText: device.displayName + " was switched on") }
    if (status == "switchingOn") { sendEvent(name: "switch", value: "off", display: true, descriptionText: device.displayName + " requested to switch on") }
    if (status == "switchingOff") { sendEvent(name: "switch", value: "on", display: true, descriptionText: device.displayName + " requested to switch on") }
}
