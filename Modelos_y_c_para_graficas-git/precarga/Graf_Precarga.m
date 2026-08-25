% =========================================================================
% Código para graficar resultados de Precarga del TFG
% =========================================================================

% 1. Extraer los datos del Workspace (Simulink exporta en una variable 'out')
tiempo = out.sim_V.Time;
voltaje = out.sim_V.Data;
corriente = out.sim_I.Data;

% =========================================================================
% FIGURA 1: Tensión en el Inversor
% =========================================================================
fig1 = figure('Name', 'Voltaje de Precarga', 'Color', 'w');
plot(tiempo, voltaje, 'b-', 'LineWidth', 2); % Línea azul gruesa

% Formato profesional
grid on;
title('Evolución de la Tensión en el Inversor', 'FontSize', 12, 'FontName', 'Times New Roman');
xlabel('Tiempo (s)', 'FontSize', 11, 'FontName', 'Times New Roman');
ylabel('Tensión (V)', 'FontSize', 11, 'FontName', 'Times New Roman');
xlim([0 3]); 
ylim([-10 130]); % Margen extra para que se vea bien el 120V
set(gca, 'FontSize', 10, 'FontName', 'Times New Roman', 'GridAlpha', 0.3);

% =========================================================================
% FIGURA 2: Corriente de Precarga
% =========================================================================
fig2 = figure('Name', 'Corriente de Precarga', 'Color', 'w');
plot(tiempo, corriente, 'r-', 'LineWidth', 2); % Línea roja gruesa

% Formato profesional
grid on;
title('Corriente de inserción (Inrush Current)', 'FontSize', 12, 'FontName', 'Times New Roman');
xlabel('Tiempo (s)', 'FontSize', 11, 'FontName', 'Times New Roman');
ylabel('Corriente (A)', 'FontSize', 11, 'FontName', 'Times New Roman');
xlim([0 3]);
% Nota: Si haces la simulación de la "Catástrofe", MATLAB ajustará el eje Y solo.
set(gca, 'FontSize', 10, 'FontName', 'Times New Roman', 'GridAlpha', 0.3);