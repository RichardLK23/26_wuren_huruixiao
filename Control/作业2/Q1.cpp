#include <iostream>
#include <Eigen/Dense>

Eigen::Vector2d X(0.0, 0.0);
Eigen::Vector2d target(3.0, 3.0);

double eta = 0.18; // 超参数学习率
double tolerance = 1e-3;
int max_cnt = 1000;
int cnt = 0;

int main() {
    printf("开始梯度下降寻优...\n");

    // 2. 迭代循环
    while (cnt < max_cnt) {
        // 计算当前误差
        double error = (X - target).norm();
        if (error < tolerance) {
            break;
        }

        // 计算梯度
        Eigen::Vector2d grad;
        grad(0) = X(0) - 3.0;         // df/dx
        grad(1) = 10.0 * (X(1) - 3.0); // df/dy

        // 梯度下降更新
        X = X - eta * grad;
        cnt ++;

        printf("cnt %d: Pos = (%.6f, %.6f), Error = %.6f\n", cnt, X(0), X(1), error);
    }

    printf("\n【结果】迭代次数: %d, 最终坐标: (%.6f, %.6f)\n", cnt, X(0), X(1));
    return 0;
}