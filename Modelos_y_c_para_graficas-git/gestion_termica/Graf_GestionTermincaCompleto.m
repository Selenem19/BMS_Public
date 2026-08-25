% =========================================================================
% Script de Validación Térmica Dinámica (Ciclo de Conducción) - TFG
% =========================================================================

% 1. Extracción de datos (Asegúrate de haber añadido out.Corriente en Simulink)
t = out.Temperatura.Time;
temp = out.Temperatura.Data;
pwm = out.PWM.Data;
sop = out.SoP.Data;
estado = out.Estado.Data;
corriente = out.Corriente.Data; 

% 2. Configuración de la figura principal
fig = figure('Name', 'Ciclo_Conduccion_BMS', 'Color', 'w', 'Position', [100, 50, 900, 1000]);

% --- PANEL 1: CORRIENTE DE ENTRADA (DEMANDA DEL CONDUCTOR) ---
ax1 = subplot(5,1,1);
plot(t, corriente, 'LineWidth', 2, 'Color', [0.2 0.2 0.2]); % Gris oscuro
grid on; hold on;
yline(0, 'k:', 'LineWidth', 1); % Línea de referencia cero
ylabel('Corriente (A)', 'FontWeight', 'bold');
title('Perfil de Misión Dinámico (Entrada del Sistema)', 'FontSize', 11);
legend({'Corriente del Pack'}, 'Location', 'northeast');

% --- PANEL 2: TEMPERATURA ---
ax2 = subplot(5,1,2);
plot(t, temp, 'LineWidth', 2.5, 'Color', [0.8500, 0.3250, 0.0980]);
grid on; hold on;
yline(35, 'k--', 'Inicio Adveccion (35^\circC)', 'LabelHorizontalAlignment', 'left');
yline(50, 'k--', 'Inicio Derating (50^\circC)', 'LabelHorizontalAlignment', 'left');
yline(60, 'r--', 'Emergencia (60^\circC)', 'LabelHorizontalAlignment', 'left', 'Color', 'r');
ylabel('Temperatura (^\circC)', 'FontWeight', 'bold');
ylim([18, 65]);

% --- PANEL 3: PWM (VENTILADORES) ---
ax3 = subplot(5,1,3);
plot(t, pwm, 'LineWidth', 2, 'Color', [0, 0.4470, 0.7410]);
grid on;
ylabel('PWM Extractores (%)', 'FontWeight', 'bold');
ylim([-5, 105]);

% --- PANEL 4: STATE OF POWER (SoP) ---
ax4 = subplot(5,1,4);
plot(t, sop, 'LineWidth', 2, 'Color', [0.4660, 0.6740, 0.1880]);
grid on;
ylabel('Potencia SoP (%)', 'FontWeight', 'bold');
ylim([-5, 105]);

% --- PANEL 5: MÁQUINA DE ESTADOS ---
ax5 = subplot(5,1,5);
plot(t, estado, 'LineWidth', 2, 'Color', [0.4940, 0.1840, 0.5560]);
grid on;
ylabel('Estado Activo', 'FontWeight', 'bold');
xlabel('Tiempo de simulación (s)', 'FontWeight', 'bold');
yticks([1, 2, 3, 4]);
yticklabels({'1: Pasivo', '2: Advección', '3: Derating', '4: Fallo'});
ylim([0.5, 4.5]);

% Sincronizar todos los ejes X
linkaxes([ax1, ax2, ax3, ax4, ax5], 'x');
xlim(ax1, [0, max(t)]);

sgtitle('Validación del Lazo Cerrado Térmico ante Ciclo de Conducción Dinámico', 'FontSize', 13, 'FontWeight', 'bold');