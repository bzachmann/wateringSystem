# wateringSystem

Known issues:
- Uses uint32_t for keeping track of unix time.  Will stop working in 2038
- If RTC stops working, the scheduled waterings will be missed
- On startup, compile time values are used for the watering schedule until mqtt commands are received
   - On a restart (power outage), an extra watering cycle could occur if the compile time values satisfy conditions for time to water.
   - The timeUntilScheduledWatering value is calculated based on compile time values for watering schedule.  This is published to mqtt server when connected.
     It is almost immediately changed once the mqtt commands are received and the timeUntilScheduledWatering is recalculated based on the mqtt values.

Potential Improvements:
- Get rid of RTC
  - Use Network Time Protocol(NTP) to get initial time
  - Use internal timer to keep track of seconds
  - Use NTP to sync the internal timer every so often