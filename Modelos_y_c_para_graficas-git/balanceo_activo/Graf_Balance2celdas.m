% =========================================================================
% SCRIPT PARA DIBUJAR LAS GRÁFICAS DE BALANCEO ACTIVO (TFG) - ZOOM
% =========================================================================

tiempo = out.Balance.Time;
corriente = out.Balance.Data;
v_celda1 = out.VolBat1.Data;
v_celda2 = out.VolBat2.Data;

fig = figure('Color', 'w', 'Name', 'Resultados de Balanceo', 'Position', [100, 100, 800, 600]);

% -------------------------------------------------------------------------
% SUBPLOT 1: Corriente del Inductor (CON ZOOM)
% -------------------------------------------------------------------------
subplot(2, 1, 1);
plot(tiempo, corriente, 'Color', '#D95319', 'LineWidth', 1.5); 
grid on;
box on;

title('Corriente del Inductor (I_L)', 'FontWeight', 'bold', 'FontSize', 12);
ylabel('Corriente [A]', 'FontWeight', 'bold');
xlabel('Tiempo [s]');
legend('Corriente de Balanceo', 'Location', 'best');
set(gca, 'FontSize', 11, 'GridAlpha', 0.4);

% COMANDO DE ZOOM EN EL EJE X (Muestra solo 50 microsegundos al final)
%xlim([0.0048, 0.00485]); 

% -------------------------------------------------------------------------
% SUBPLOT 2: Tensión de las Celdas (CON ZOOM)
% -------------------------------------------------------------------------
subplot(2, 1, 2);
plot(tiempo, v_celda1, 'Color', '#0072BD', 'LineWidth', 1.5); 
hold on;
plot(tiempo, v_celda2, 'Color', '#77AC30', 'LineWidth', 1.5); 
grid on;
box on;

title('Evolución de Tensión en las Celdas', 'FontWeight', 'bold', 'FontSize', 12);
ylabel('Tensión [V]', 'FontWeight', 'bold');
xlabel('Tiempo [s]', 'FontWeight', 'bold');
legend('Celda Transmisora (4.2 V)', 'Celda Receptora (3.0 V)', 'Location', 'best');
set(gca, 'FontSize', 11, 'GridAlpha', 0.4);
hold off;

% COMANDO DE ZOOM EN EL EJE X (Debe ser idéntico al de arriba)
%xlim([0.0048, 0.00485]); 

% =========================================================================