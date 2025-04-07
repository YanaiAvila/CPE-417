% Channel settings
channelID = 2741565; % Channel ID
readAPIKey = '9PDIEA2A1OBL2MBN'; % Read API key

% Fetch data from the last 10 points
data = thingSpeakRead(channelID, 'Fields', [1, 2], 'NumPoints', 10, 'ReadKey', readAPIKey);

% Check if data is retrieved
if isempty(data)
    disp('No data retrieved.');
else
    % If it is, calculate average light level and soil moisture
    avgLightLevel = mean(data(:, 1), 'omitnan');
    avgSoilMoisture = mean(data(:, 2), 'omitnan');

    % Output averages to terminal
    fprintf('Average Light Level: %.2f\n', avgLightLevel);
    fprintf('Average Soil Moisture: %.2f\n', avgSoilMoisture);
end