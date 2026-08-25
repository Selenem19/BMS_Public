% --- Script para visualización unificada (Escala -50 a 110) ---

% Extracción de datos (usando el método robusto)
if isstruct(out.Corriente)
    t = out.Corriente.time;
    curr = out.Corriente.signals.values;
else
    t = out.Corriente.Time;
    curr = out.Corriente.Data;
end

if isstruct(out.sim_SoC)
    soc = out.sim_SoC.signals.values;
else
    soc = out.sim_SoC.Data;
end

% Crear figura
figure('Color', 'w');

% Plot simple para que compartan el mismo eje Y
plot(t, soc, 'LineWidth', 2.5, 'Color', [0 0.447 0.741]); % Azul (SoC)
hold on;
plot(t, curr, 'LineWidth', 1.5, 'Color', [0.85 0.325 0.098]); % Naranja (Corriente)

% --- ESCALADO FIJO EN AMBOS ---
ylim([-50 110]);

% Etiquetas y estética
xlabel('Tiempo de simulacion (s)');
ylabel('Valor (SoC % / Corriente A)');
title('Validacion del Estimador de SoC');
grid on;
legend({'SoC (%)', 'Corriente (A)'}, 'Location', 'northeast');

