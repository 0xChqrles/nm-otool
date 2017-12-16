var fs = require('fs'),
	exec = require('child_process').exec;

var	p, size, i = 0, file = null, step = 1;

while (i++ < process.argv.length - 1)
{
	if (process.argv[i] == "-f")
		file = process.argv[++i];
	else if (process.argv[i] == "-s")
		step = Number(process.argv[++i]);
}
if (!file)
{
	console.log("-f <file> -s <step>");
	process.exit();
}
size = fs.statSync(file).size;

test_size(size);

function	test_size(size)
{
	p = exec("cat " + file + " | head -c" + size + " > tmp; ./ft_nm tmp");
	p.on('close', function(data, signal) {
		if (Number(`${data}`) > 1)
		{
			console.log("[KO] " + Number(`${data}`) + " : " + size);
			process.exit();
		}
		else
			console.log("[OK] : " + size);
		if ((size -= step) > 0)
			test_size(size);
	});
}
