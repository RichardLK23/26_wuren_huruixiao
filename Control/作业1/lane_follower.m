%% 第二问：轨迹跟踪
clear; clc; close all

% 车辆参数
lfr = 2.168 + 1.907; % 轴距 L
dt = 0.01;
v = 15; 
sim_steps = 2000;

% 参考轨迹 (正弦曲线)
X_ref = 0:0.1:200; 
Y_ref = 10 * sin(X_ref / 15); 

% 初始车辆状态 
X = X_ref(1); Y = Y_ref(1) + 3; phi = 0; 
X_vec = zeros(1, sim_steps); Y_vec = zeros(1, sim_steps);

% 超参数Ld
Ld = 5.0;

for ii = 1:sim_steps
    X_vec(ii) = X; Y_vec(ii) = Y;
    
    
    % ===============================================================
    
    % ================= TODO 2.1: 实现某种跟踪算法 =================
    
    % 1. 寻找参考轨迹上距离当前车辆最近的点，以及对应的目标前视点
    distances = sqrt((X_ref - X).^2 + (Y_ref - Y).^2);
    [~, min_idx] = min(distances); % 车辆当前对应的最接近的轨迹点索引
    
    % 2. 从最近点开始向后搜索，找到距离等于或最接近 Ld 的前视目标点
    target_idx = min_idx;
    for jj = min_idx:length(X_ref)
        if distances(jj) >= Ld
            target_idx = jj;
            break;
        end
    end
    X_target = X_ref(target_idx);
    Y_target = Y_ref(target_idx);
    
    % 3. 计算目标点在小车局部坐标系下的夹角 alpha
    % 目标点相对于小车的绝对方位角
    theta = atan2(Y_target - Y, X_target - X);
    % 夹角 alpha = 绝对方位角 - 小车自身的航向角
    alpha = theta - phi;
    
    % 4. 根据公式计算前轮转向角 sigma
    sigma = atan2(2 * lfr * sin(alpha), Ld);

    % ===============================================================

    % ================= TODO 2.2: 车辆状态更新 =================
    % 提示: 将刚才求得的转向角 sigma 代入运动学模型（复用第一问代码），更新 X, Y, phi。
    
    phi_dot = (v * tan(sigma)) / lfr;
    X = X + v * cos(phi) * dt;
    Y = Y + v * sin(phi) * dt;
    phi = phi + phi_dot * dt;
    
    % ===============================================================
    
    % 到达终点提前结束
    if X >= X_ref(end), break; end
end

% 绘图对比
figure; hold on; grid on;
plot(X_ref, Y_ref, 'w--', 'LineWidth', 2);
plot(X_vec(1:ii), Y_vec(1:ii), 'r-', 'LineWidth', 2);
legend('参考规划轨迹', '实际行驶轨迹');
title(['Pure Pursuit 跟踪 (Ld = ', num2str(Ld), 'm)']);
xlabel('X [m]'); ylabel('Y [m]'); axis equal;