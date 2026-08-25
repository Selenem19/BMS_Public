% =========================================================================
% Código para graficar resultados de Control Térmico (Histéresis)
% =========================================================================

% 1. Extraer los datos del Workspace
tiempo = out.sim_T.Time;
temperatura = out.sim_T.Data;

% =========================================================================
% FIGURA 3: Evolución Térmica con Advección Activa
% =========================================================================
fig3 = figure('Name', 'Control Térmico', 'Color', 'w');
plot(tiempo, temperatura, 'k-', 'LineWidth', 2); % Línea principal en negro
hold on;

% Añadir líneas de los umbrales de histéresis
yline(35, 'r--', 'Umbral Encendido (35 °C)', 'LineWidth', 1.5, 'LabelHorizontalAlignment', 'left', 'FontName', 'Times New Roman');
yline(32, 'b--', 'Umbral Apagado (32 °C)', 'LineWidth', 1.5, 'LabelHorizontalAlignment', 'left', 'FontName', 'Times New Roman');

% Formato profesional
grid on;
title('Dinámica Térmica del Paquete de Baterías (Estrategia Híbrida)', 'FontSize', 12, 'FontName', 'Times New Roman');
xlabel('Tiempo (s)', 'FontSize', 11, 'FontName', 'Times New Roman');
ylabel('Temperatura (°C)', 'FontSize', 11, 'FontName', 'Times New Roman');
xlim([0 250]); 
ylim([18 38]); % Ajustado para ver los márgenes de seguridad
set(gca, 'FontSize', 10, 'FontName', 'Times New Roman', 'GridAlpha', 0.3);
hold off;