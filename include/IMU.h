
class MPU{
    public:
    int16_t accX,accY,accZ;
    int16_t gyroX,gyroY,gyroZ;

    void init(void);
    void readAcc(void);
    void readGyro(void);
    int16_t  getAccX(void);
    int16_t  getAccY(void);
    int16_t  getAccZ(void);
} ;