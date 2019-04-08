Range = LogTest.Max - LogTest.Min;
close all
figure(1);
plot(LogTest.Avg.*1.2534, LogTest.Tally);
hold on
errorbar(LogTest.Avg.*1.2534, LogTest.Avg, Range, 'rd');
xlabel("Wind Speed [m/s]");
ylabel("Frequency [Hz]");

Error = abs(((LogTest.Tally - LogTest.Avg)./LogTest.Avg)*100);
ErrorAvg = polyfit(LogTest.Avg.*1.2534, Error, 1);
figure(2);
subplot(2,1,1)
plot(LogTest.Avg.*1.2534, Error, 'k*', 'MarkerSize',10);
hold on
plot(LogTest.Avg.*1.2534, polyval(ErrorAvg, LogTest.Avg.*1.2534));
ylabel("Error [%]");
xlabel("Wind Speed [m/s]");
title("Percent Error Measured vs. Windspeed (Tally)");

FreqRange = linspace(LogTest.Avg(1), LogTest.Avg(4), 4);
ALogError = (1 - (4 - 1./FreqRange)./4)*100;
TallyError = (1 - (60 - 1./FreqRange)./60)*100;
subplot(2,1,2)
plot(LogTest.Avg.*1.2534, ALogError);
hold on
plot(LogTest.Avg.*1.2534, TallyError);
ylabel("Error [%]");
xlabel("Wind Speed [m/s]");
title("Percent Error Theoretical vs. Windspeed");
legend("ALog","Tally");

