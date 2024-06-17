1. sample 0 和 sample 1 是演示mcf 标定mpi接口，sample 2 和 sample 3 是演示 mcf 基本通路。
2. sample 2 和 sample 3 支持IMX347/os08a20 sensor ,使用 os08a20时，由于sensor是主模式，不能保证采集2路数据时间完全同步，所以sample效果可能不正确。
3. sample 2 和 sample 3 使用的分光棱镜模组镜头，如果镜头有误差，可以使用mipi crop 来对齐2个sensor 视场。
4. 在2路sensor有视差的场景时，需要使用标定接口标定，标定接口和标定结果使用，请参考《黑白彩色双路融合 开发参考》和 《SS928V100 黑白彩色双路融合调试指南》
