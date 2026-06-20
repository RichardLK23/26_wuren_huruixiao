%% 第二问：轨迹跟踪 (PID 控制)
clear; clc; close all

% 车辆参数 (复用第一问)
lf = 2.168; 
lr = 1.907; 
lfr = lf + lr; % 轴距 L
dt = 0.01;
v = 15;        % 车速 15 m/s
sim_steps = 2000;

% 参考轨迹 (正弦曲线)
X_ref = 0:0.1:200; 
Y_ref = 10 * sin(X_ref / 15); 
% 计算参考轨迹每个点的切线角（用于航向角对齐）
phi_ref = gradient(Y_ref, 0.1); 

% 初始车辆状态 (初始偏离轨道 3 米)
X = X_ref(1); Y = Y_ref(1) + 3; phi = 0; 

X_vec = zeros(1, sim_steps); Y_vec = zeros(1, sim_steps);

% ================= PID 参数初始化 =================
Kp = 0.40;    % 比例系数：决定对当前误差的修正力度
Ki = 0.01;    % 积分系数：消除静差（如果车总是差一点点对不准）
Kd = 0.15;    % 微分系数：阻尼作用，防止车头画龙摆动

error_integral = 0;  % 误差积分累加器
error_last = 0;      % 上一次的误差
% =================================================

for ii = 1:sim_steps
    X_vec(ii) = X; Y_vec(ii) = Y;
    
    % ===============================================================
    % ================= TODO 2.1: 实现某种跟踪算法 (PID) =================
    
    % 1. 寻找参考轨迹上距离当前车辆最近的点
    distances = sqrt((X_ref - X).^2 + (Y_ref - Y).^2);
    [~, min_idx] = min(distances); 
    
    % 2. 计算横向误差 ey (考虑正负号)
    % 如果车辆在轨迹上方，误差为正；在下方为负（这里用简单的 Y 轴差值近似横向误差）
    error_y = Y_vec(ii) - Y_ref(min_idx); 
    
    % 3. PID 三项计算
    error_integral = error_integral + error_y * dt;          % 积分
    error_derivative = (error_y - error_last) / dt;         % 微分
    error_last = error_y;                                    % 更新历史
    
    % 4. 计算基本的转向角（根据横向误差）
    sigma_input = -(Kp * error_y + Ki * error_integral + Kd * error_derivative);
    
    % 5. 加上航向角前馈（车辆航向与轨迹切线方向的偏差修正）
    target_phi = atan(phi_ref(min_idx));
    error_phi = target_phi - phi;
    
    % 航向角误差角度归一化到 [-pi, pi]
    error_phi = atan2(sin(error_phi), cos(error_phi)); 
    
    % 最终控制输出 = 横向误差PID + 航向误差P控制
    sigma = sigma_input + 0.5 * error_phi;
    
    % 限制前轮最大转角（现实中方向盘不能无限打，限制在正负 35 度内）
    max_sigma = 35 / 180 * pi;
    sigma = max(min(sigma, max_sigma), -max_sigma);
    
    % ===============================================================
    % ================= TODO 2.2: 车辆状态更新 =================
    
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
plot(X_vec(1:ii), Y_vec(1:ii), 'b-', 'LineWidth', 2); % PID 用蓝色表示
legend('参考规划轨迹', 'PID实际行驶轨迹');
title(['PID 轨迹跟踪仿真 (Kp=', num2str(Kp), ', Kd=', num2str(Kd), ')']);
xlabel('X [m]'); ylabel('Y [m]'); axis equal;
xlim([-10, 210]); ylim([-20, 20]);
