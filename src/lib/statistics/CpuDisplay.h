void cpuDisplay()
{
    const float maxTicksPerUpdate = (((float)F_CPU) / 44100.0f) * 128.0f;
    AudioStream *p;
    M5.Lcd.printf("%i ms uptime\r\n\r\n", millis());
    M5.Lcd.printf("CPU 0 %5.2f%% [%5.2f%% max]  \r\n", cpu0Load, cpu0LoadMax);
    M5.Lcd.printf("CPU 1 %5.2f%% [%5.2f%% max]  \r\n\r\n", cpu1Load, cpu1LoadMax);
    int clocks, clocksMax;
    float load, loadMax;
    int totalClocks, totalClocksMax;
    M5.Lcd.printf("%-31s %6s %7s %6s\r\n", "Audio Object", "CPU %", "Clocks", "Max %"); 
    M5.Lcd.printf("-----------------------------------------------------\r\n");
    for (p = AudioStream::first_update; p; p = p->next_update) {
        if (p->active) {
            if(!p->blocking){
                clocks = p->clocksPerUpdate;
                clocksMax = p->clocksPerUpdateMax;
                load = 100.0f * ((float)clocks / maxTicksPerUpdate);
                loadMax = 100.0f * ((float)clocksMax / maxTicksPerUpdate);
                M5.Lcd.printf("%-31s %6.2f %7i %6.2f\r\n", p->name, load, clocks, loadMax);
                totalClocks += clocks;
                totalClocksMax += clocksMax;
            }	
            else{
                M5.Lcd.printf("%-31s %6s %7s %6s\r\n", p->name, "-", "-", "-");
            }		
        }
    }
    M5.Lcd.printf("                                ---------------------\r\n");
    M5.Lcd.printf("%31s %6.2f %7i %6.2f\r\n", "", 100.0f * ((float)totalClocks / maxTicksPerUpdate), totalClocks, 100.0f * ((float)totalClocksMax / maxTicksPerUpdate));
}