var entropy = new TimeSeries();
var limit = new TimeSeries();

$(function() {
    $("#status").text(""); 
    initChart();
    get_entropy();
    setInterval(function() { get_entropy(); limit.append(new Date().getTime(), 100); }, 250);
});

function initChart()
{
    var chart = new SmoothieChart({ maxValueScale:1.5 });

    chart.addTimeSeries(entropy,
    { 
        strokeStyle: 'rgba(0, 255, 0, 1)',
        fillStyle: 'rgba(0, 255, 0, 0.2)',
        lineWidth: 2
    });

    chart.addTimeSeries(limit,
    { 
        strokeStyle: 'rgba(255, 0, 0, 1)',
        fillStyle: 'rgba(0, 255, 0, 0.2)',
        lineWidth: 2
    });
    chart.streamTo(document.getElementById("chart"), 250);
}

function get_entropy()
{
   $.ajax(
   {
       url: "entropy.api",
       type: "post",
   }).done(function(response)
   {
       $("#entropyinfo").text(response);
       var e = Number(response);
       entropy.append(new Date().getTime(), e);
   });
}
