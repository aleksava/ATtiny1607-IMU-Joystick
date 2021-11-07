
class MPU{
    public:
    float accX,accY,accZ;
    float accXerr,accYerr,accZerr;
    float accangleX, accangleY;
    float gyroX,gyroY,gyroZ;
    float gyroXerr,gyroYerr,gyroZerr;
    float gyroangleX,gyroangleY;
    float pitch, roll, yaw;
    uint16_t previous_time_ms;
    uint16_t current_time_ms;
    float elapsedTime;
    uint8_t calibration_done;

    void init(void);
    void calibrate(void);
    void readAcc(void);
    void readGyro(void);
    void calculate_roation(void);
    void print_orientation(void);
    float getPitch(void);
    float getRoll(void);
    float getYaw(void);
    void sleep(void);
    void wake(void);
} ;